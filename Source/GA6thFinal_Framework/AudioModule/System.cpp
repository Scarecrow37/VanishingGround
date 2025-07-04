#include "pch.h"
#include "System.h"

namespace Audio
{
    namespace
    {
#ifdef _XBOX
        constexpr FOURCC RIFF = MAKEFOURCC('F', 'F', 'I', 'R');
        constexpr FOURCC DATA = MAKEFOURCC('a', 't', 'a', 'd');
        constexpr FOURCC FMT  = MAKEFOURCC(' ', 't', 'm', 'f');
        constexpr FOURCC WAVE = MAKEFOURCC('E', 'V', 'A', 'W');
        constexpr FOURCC XWMA = MAKEFOURCC('A', 'M', 'W', 'X');
        constexpr FOURCC DPDS = MAKEFOURCC('s', 'd', 'p', 'd');
#else
        constexpr FOURCC RIFF = MAKEFOURCC('R', 'I', 'F', 'F');
        constexpr FOURCC DATA = MAKEFOURCC('d', 'a', 't', 'a');
        constexpr FOURCC FMT  = MAKEFOURCC('f', 'm', 't', ' ');
        constexpr FOURCC WAVE = MAKEFOURCC('W', 'A', 'V', 'E');
        constexpr FOURCC XWMA = MAKEFOURCC('X', 'W', 'M', 'A');
        constexpr FOURCC DPDS = MAKEFOURCC('D', 'P', 'D', 'S');
#endif

        struct FindChunk
        {
            std::pair<std::streamsize, std::streamoff> operator()(std::ifstream& fileStream, const FOURCC fourcc) const
            {
                static constexpr std::streamsize FOURCC_COUNT          = sizeof(FOURCC);
                static constexpr std::streamsize CHUNK_DATA_SIZE_COUNT = sizeof(DWORD);

                // 초기화
                FOURCC format;

                std::streamsize chunkDataSize = 0;
                std::streamoff  offset        = 0;

                // 파일 포인터를 파일의 시작으로 설정.
                fileStream.seekg(0, std::ios::beg);

                while (fileStream.eof())
                {
                    fileStream.read(reinterpret_cast<char*>(&format), FOURCC_COUNT);
                    fileStream.read(reinterpret_cast<char*>(&chunkDataSize), CHUNK_DATA_SIZE_COUNT);

                    if (format == RIFF)
                    {
                        chunkDataSize = FOURCC_COUNT;
                        fileStream.seekg(FOURCC_COUNT, std::ios::cur);
                    }
                    else
                    {
                        fileStream.seekg(chunkDataSize, std::ios::cur);
                    }

                    offset += FOURCC_COUNT + CHUNK_DATA_SIZE_COUNT;

                    if (format == fourcc)
                    {
                        return {chunkDataSize, offset};
                    }

                    offset += chunkDataSize;
                }
                throw AudioException("Chunk not found: " + std::to_string(fourcc));
            }
        };

        struct ReadChunkData
        {
            void operator()(std::ifstream& fileStream, void* buffer, const std::streamsize& bufferSize,
                            const std::streamoff bufferOffset) const
            {
                fileStream.seekg(bufferOffset, std::ios::beg);
                fileStream.read(static_cast<char*>(buffer), bufferSize);
                if (fileStream.gcount() != bufferSize)
                    throw AudioException("Failed to read chunk data: " + std::to_string(bufferSize));
            }
        };

        struct ThrowIfFailed
        {
            void operator()(const HRESULT result, const std::string& message) const
            {
                if (FAILED(result))
                {
                    switch (result)
                    {
                    case XAUDIO2_E_INVALID_CALL:
                        throw InvalidCallException(message);
                    case XAUDIO2_E_XMA_DECODER_ERROR:
                        throw XmaDecoderException(message);
                    case XAUDIO2_E_XAPO_CREATION_FAILED:
                        throw EffectCreationException(message);
                    case XAUDIO2_E_DEVICE_INVALIDATED:
                        throw DeviceInvalidatedException(message);
                    default:
                        LPSTR       buffer;
                        const DWORD count = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                                               FORMAT_MESSAGE_IGNORE_INSERTS,
                                                           nullptr, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                                           reinterpret_cast<LPSTR>(&buffer), 0, nullptr);
                        std::string fullMessage = message;
                        if (buffer != nullptr)
                        {
                            fullMessage.append(buffer);
                            LocalFree(buffer);
                        }
                        throw AudioException(fullMessage);
                    }
                }
            }
        };

        struct GetWaveFormatHash
        {
            template <typename T>
            void Combine(WaveFormatHash& seed, T value) const
            {
                seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            WaveFormatHash operator()(const WAVEFORMATEX& waveFormat) const
            {
                WaveFormatHash seed = 0;
                Combine(seed, waveFormat.wFormatTag);
                Combine(seed, waveFormat.nChannels);
                Combine(seed, waveFormat.nSamplesPerSec);
                Combine(seed, waveFormat.nAvgBytesPerSec);
                Combine(seed, waveFormat.nBlockAlign);
                Combine(seed, waveFormat.wBitsPerSample);
                Combine(seed, waveFormat.cbSize);
                return seed;
            }
        };
    } // namespace

    System::System() = default;

    System::~System()
    {
        if (_masteringVoice)
        {
            _masteringVoice->DestroyVoice();
            _masteringVoice = nullptr;
        }
    }

    System::System(System&& other) noexcept
        : _xAudio2(std::move(other._xAudio2)), _masteringVoice(other._masteringVoice)
    {
        other._masteringVoice = nullptr;
    }

    System& System::operator=(System&& other) noexcept
    {
        if (this == &other)
            return *this;
        _xAudio2              = std::move(other._xAudio2);
        _masteringVoice       = other._masteringVoice;
        other._masteringVoice = nullptr;
        return *this;
    }

    void System::Initialize()
    {
        constexpr ThrowIfFailed throwIfFailed;

        throwIfFailed(XAudio2Create(_xAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR), "Failed to create XAudio2 instance");

        throwIfFailed(_xAudio2->CreateMasteringVoice(&_masteringVoice), "Failed to create mastering voice");
    }

    Source System::CreateSoundFromWave(const std::filesystem::path& filePath, const bool isLoop)
    {
        std::ifstream fileStream;
        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fileStream.open(filePath, std::ios::binary);

        constexpr FindChunk     findChunk;
        constexpr ReadChunkData readChunkData;

        // RIFF 청크
        auto [riffSize, riffPosition] = findChunk(fileStream, RIFF);

        FOURCC fileType;
        readChunkData(fileStream, &fileType, riffSize, riffPosition);

        if (fileType != WAVE)
            throw AudioException("Invalid file format: Not a WAVE file");

        // Format 청크
        auto [fmtSize, fmtPosition] = findChunk(fileStream, FMT);

        WAVEFORMATEX wfx{};
        readChunkData(fileStream, &wfx, fmtSize, fmtPosition);

        // Data 청크
        auto [dataSize, dataPosition] = findChunk(fileStream, DATA);

        BYTE* audioData = new BYTE[dataSize];
        try
        {
            readChunkData(fileStream, audioData, dataSize, dataPosition);
        }
        catch (...)
        {
            delete[] audioData;
            throw;
        }

        XAUDIO2_BUFFER buffer{};
        buffer.Flags      = XAUDIO2_END_OF_STREAM; // Indicates the end of the stream
        buffer.AudioBytes = static_cast<UINT32>(dataSize);
        buffer.pAudioData = audioData;
        if (isLoop)
        {
            buffer.LoopBegin  = 0;
            buffer.LoopLength = 0;
            buffer.LoopCount  = XAUDIO2_LOOP_INFINITE;
        }

        return Source{wfx, buffer};
    }

    Handle System::Play(const Source& sound)
    {
        if (_xAudio2 == nullptr)
            throw AudioException("Audio manager is not initialized.");

        constexpr ThrowIfFailed throwIfFailed;

        // 초기화
        const WaveFormatHash hash        = GetWaveFormatHash()(sound._format);
        Generation           generation  = 0;
        Index                index       = 0;
        IXAudio2SourceVoice* sourceVoice = nullptr;

        // Format에 맞는 Pool 찾기
        std::vector<SourceVoice>* sourceVoices = nullptr;
        if (!_sourceVoices.contains(hash)) // 없으면 생성
        {
            _sourceVoices.try_emplace(hash, std::vector<SourceVoice>{});
        }
        sourceVoices = &_sourceVoices.at(hash);

        // 미사용 Voice 찾기
        auto unusedSourceVoiceIterator = std::ranges::find_if(*sourceVoices, [](const SourceVoice& sv) {
            static IsUnusedGeneration isUnusedGeneration;
            return isUnusedGeneration(sv.Generation);
        });

        const bool notFound = unusedSourceVoiceIterator == sourceVoices->end();

        if (notFound) // 없으면 SourceVoice 생성
        {
            sourceVoices->push_back(SourceVoice{.Generation = 0, .Callback = {}, .Voice = nullptr});
            unusedSourceVoiceIterator = std::prev(sourceVoices->end());
        }

        generation = IncreaseGeneration()(unusedSourceVoiceIterator->Generation);
        index      = std::distance(sourceVoices->begin(), unusedSourceVoiceIterator);

        // Handle 생성
        const Handle handle = {hash, index, generation};

        // Callback 갱신
        SourceVoice& unusedVoice = sourceVoices->at(index);
        unusedVoice.Callback.SetOnBufferEndCallback([this, handle]() {
            if (IsValidHandle(handle))
                ReleaseVoice(handle);
        });

        // 필요시 IXAudio2SourceVoice 생성
        if (notFound)
        {
            throwIfFailed(_xAudio2->CreateSourceVoice(&unusedVoice.Voice, &sound._format, 0, XAUDIO2_DEFAULT_FREQ_RATIO,
                                                      &unusedVoice.Callback),
                          "Failed to create source voice.");
        }

        sourceVoice = unusedVoice.Voice;

        // Submit 후 시작
        throwIfFailed(sourceVoice->SubmitSourceBuffer(&sound._buffer), "Failed to submit source buffer.");
        throwIfFailed(sourceVoice->Start(0), "Failed to start source voice.");

        return handle;
    }

    void System::Stop(const Handle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to Stop.");

        constexpr ThrowIfFailed throwIfFailed;

        const SourceVoice& sourceVoice = _sourceVoices.at(handle._hash).at(handle._index);
        throwIfFailed(sourceVoice.Voice->Stop(0), "Failed to stop source voice.");

        ReleaseVoice(handle);
    }

    bool System::IsValidHandle(const Handle& handle) const noexcept
    {
        return _sourceVoices.contains(handle._hash) && handle._index >= 0 &&
               static_cast<decltype(_sourceVoices.size())>(handle._index) < _sourceVoices.size() &&
               handle._generation == _sourceVoices.at(handle._hash).at(handle._index).Generation;
    }

    void System::ReleaseVoice(const Handle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to ReleaseVoice.");

        constexpr ThrowIfFailed      throwIfFailed;
        constexpr IncreaseGeneration increaseGeneration;

        SourceVoice& sourceVoice = _sourceVoices.at(handle._hash).at(handle._index);
        throwIfFailed(sourceVoice.Voice->FlushSourceBuffers(), "Failed to flush source buffers.");

        increaseGeneration(sourceVoice.Generation);
    }

} // namespace Audio