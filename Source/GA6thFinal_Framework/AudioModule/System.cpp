#include "pch.h"
#include "System.h"

#include <ranges>

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

                while (!fileStream.eof())
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

            WaveFormatHash operator()(const WAVEFORMATEXTENSIBLE& waveFormat) const
            {
                WaveFormatHash seed = 0;
                Combine(seed, waveFormat.Format.wFormatTag);
                Combine(seed, waveFormat.Format.nChannels);
                Combine(seed, waveFormat.Format.nSamplesPerSec);
                Combine(seed, waveFormat.Format.nAvgBytesPerSec);
                Combine(seed, waveFormat.Format.nBlockAlign);
                Combine(seed, waveFormat.Format.wBitsPerSample);
                Combine(seed, waveFormat.Format.cbSize);
                Combine(seed, waveFormat.Samples.wValidBitsPerSample);
                Combine(seed, waveFormat.dwChannelMask);
                Combine(seed, waveFormat.SubFormat.Data1);
                Combine(seed, waveFormat.SubFormat.Data2);
                Combine(seed, waveFormat.SubFormat.Data3);
                unsigned long long data4;
                std::memcpy(&data4, waveFormat.SubFormat.Data4, sizeof(data4));
                Combine(seed, data4);
                return seed;
            }
        };
    } // namespace

    System::System() = default;


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

    System::~System() = default;

    void System::Initialize(const bool isDebug)
    {
        constexpr ThrowIfFailed throwIfFailed;

        throwIfFailed(XAudio2Create(&_xAudio2, isDebug ? XAUDIO2_DEBUG_ENGINE : 0, XAUDIO2_DEFAULT_PROCESSOR),
                      "Failed to create XAudio2 instance");

        throwIfFailed(_xAudio2->CreateMasteringVoice(&_masteringVoice), "Failed to create mastering voice");
    }

    void System::Finalize()
    {
        ClearVoicePool();

        if (_masteringVoice)
        {
            _masteringVoice->DestroyVoice();
            _masteringVoice = nullptr;
        }

        if (_xAudio2)
        {
            _xAudio2->Release();
            _xAudio2 = nullptr;
        }
    }

    void System::ClearVoicePool()
    {
        for (auto& voiceVector : _voicePools | std::views::values)
        {
            for (auto& sourceVoice : voiceVector)
            {
                if (sourceVoice.Voice)
                {
                    sourceVoice.Voice->DestroyVoice();
                    sourceVoice.Voice = nullptr;
                }
            }
            voiceVector.clear();
        }
        _voicePools.clear();
    }

    void System::TurnOnDebugMode() const
    {
        XAUDIO2_DEBUG_CONFIGURATION debugConfig{};
        debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_INFO | XAUDIO2_LOG_DETAIL |
                                XAUDIO2_LOG_API_CALLS | XAUDIO2_LOG_FUNC_CALLS;
        _xAudio2->SetDebugConfiguration(&debugConfig);
    }

    void System::TurnOffDebugMode() const
    {
        constexpr XAUDIO2_DEBUG_CONFIGURATION debugConfig{};
        _xAudio2->SetDebugConfiguration(&debugConfig);
    }

    Source System::CreateSoundFromWave(const std::filesystem::path& filePath)
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

        WAVEFORMATEXTENSIBLE wfx{};
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

        return Source{wfx, audioData, static_cast<UINT32>(dataSize)};
    }

    AudioHandle System::Play(const Source& sound, const bool isLoop)
    {
        if (_xAudio2 == nullptr)
            throw AudioException("Audio manager is not initialized.");

        constexpr ThrowIfFailed throwIfFailed;

        // Format 해싱
        const WaveFormatHash hash = GetWaveFormatHash()(sound._format);


        // Format에 맞는 Pool 찾기
        auto [iter, succeed] = _voicePools.try_emplace(hash, VoicePool{});

        if (succeed)
        {
            iter->second.resize(MAX_POOL_SIZE, SourceVoice{.Generation = 0, .Callback = Callback(OnBufferEnd(this)), .Voice = nullptr});
        }

        VoicePool& sourceVoices = iter->second;

        // 미사용 Voice 찾기
        auto unusedSourceVoiceIterator = std::ranges::find_if(sourceVoices, [](const SourceVoice& sv) {
            static IsUnusedGeneration isUnusedGeneration;
            return isUnusedGeneration(sv.Generation);
        });

        if (unusedSourceVoiceIterator == sourceVoices.end())
        {
            throw AudioException("No available source voices in the pool.");
        }

        auto& [unusedSourceVoiceGeneration, unusedSourceVoiceCallback, unusedSourceVoice] = *unusedSourceVoiceIterator;

        IncreaseGeneration()(unusedSourceVoiceGeneration);
        const Index index = std::distance(sourceVoices.begin(), unusedSourceVoiceIterator);

        // AudioHandle 생성
        const AudioHandle handle = {hash, index, unusedSourceVoiceGeneration};
        // Callback 갱신
        unusedSourceVoiceCallback.SetHandle(handle);

        // 필요시 IXAudio2SourceVoice 생성
        if (nullptr == unusedSourceVoice)
        {
            throwIfFailed(_xAudio2->CreateSourceVoice(&unusedSourceVoice,
                                                      reinterpret_cast<const WAVEFORMATEX*>(&sound._format), NULL,
                                                      XAUDIO2_DEFAULT_FREQ_RATIO, &unusedSourceVoiceCallback),
                          "Failed to create source voice.");
        }

        // XAUDIO2_BUFFER 설정
        XAUDIO2_BUFFER buffer{};
        buffer.Flags      = NULL;
        buffer.AudioBytes = sound._bytes;
        buffer.pAudioData = sound._buffer;
        buffer.PlayBegin  = 0;
        buffer.PlayLength = 0; // 0이면 전체 재생
        buffer.LoopBegin  = 0;
        buffer.LoopLength = 0; // 0이면 전체 루프
        buffer.LoopCount  = isLoop ? XAUDIO2_LOOP_INFINITE : XAUDIO2_NO_LOOP_REGION;
        buffer.pContext   = nullptr;

        // Submit 후 시작
        throwIfFailed(unusedSourceVoice->SubmitSourceBuffer(&buffer), "Failed to submit source buffer.");
        throwIfFailed(unusedSourceVoice->Start(0), "Failed to start source voice.");

        return handle;
    }

    void System::Stop(const AudioHandle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to Stop.");

        constexpr ThrowIfFailed throwIfFailed;

        const SourceVoice& sourceVoice = _voicePools.at(handle._hash).at(handle._index);
        throwIfFailed(sourceVoice.Voice->Stop(0), "Failed to stop source voice.");

        ReleaseVoice(handle);
    }

    bool System::IsValidHandle(const AudioHandle& handle) const noexcept
    {
        if (_voicePools.contains(handle._hash))
        {
            const VoicePool& voicePool = _voicePools.at(handle._hash);
            const bool       isValidIndex =
                handle._index >= 0 && static_cast<decltype(voicePool.size())>(handle._index) < voicePool.size();
            if (isValidIndex)
            {
                const SourceVoice& sourceVoice = voicePool.at(handle._index);
                return handle._generation == sourceVoice.Generation;
            }
        }
        return false;
    }

    void System::ReleaseVoice(const AudioHandle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to ReleaseVoice.");

        constexpr ThrowIfFailed      throwIfFailed;
        constexpr IncreaseGeneration increaseGeneration;

        auto& [generation, callback, voice] = _voicePools.at(handle._hash).at(handle._index);
        throwIfFailed(voice->FlushSourceBuffers(), "Failed to flush source buffers.");
        callback.SetHandle(AudioHandle());
        increaseGeneration(generation);
    }

    System::OnBufferEnd::OnBufferEnd(Audio::System* system) : System(system) {}

    void System::OnBufferEnd::operator()(const AudioHandle& handle) const
    {
        if (nullptr != System && System->IsValidHandle(handle))
        {
            System->ReleaseVoice(handle);
        }
    }

} // namespace Audio