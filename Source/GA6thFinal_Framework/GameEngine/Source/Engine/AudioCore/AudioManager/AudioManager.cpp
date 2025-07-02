#include "pch.h"
#include "AudioManager.h"
#include "Engine/AudioCore/Declare/RIFF/AudioChunk.h"
#include "Engine/AudioCore/Source/AudioSource.h"

namespace Audio
{
    EManager::EManager() = default;

    EManager::~EManager()
    {
        if (_masteringVoice)
        {
            _masteringVoice->DestroyVoice();
            _masteringVoice = nullptr;
        }
    }

    EManager::EManager(EManager&& other) noexcept
        : _xAudio2(std::move(other._xAudio2)), _masteringVoice(other._masteringVoice)
    {
        other._masteringVoice = nullptr;
    }

    EManager& EManager::operator=(EManager&& other) noexcept
    {
        if (this == &other)
            return *this;
        _xAudio2              = std::move(other._xAudio2);
        _masteringVoice       = other._masteringVoice;
        other._masteringVoice = nullptr;
        return *this;
    }

    void EManager::Initialize()
    {
        constexpr ThrowIfFailed throwIfFailed;

        throwIfFailed(XAudio2Create(_xAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR), "Failed to create XAudio2 instance");

        throwIfFailed(_xAudio2->CreateMasteringVoice(&_masteringVoice), "Failed to create mastering voice");
    }

    Source EManager::CreateSoundFromWave(const std::filesystem::path& filePath, const bool isLoop)
    {
        std::ifstream fileStream;
        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fileStream.open(filePath, std::ios::binary);

        constexpr FindChunk     findChunk;
        constexpr ReadChunkData readChunkData;

        // RIFF 청크
        auto [riffSize, riffPosition] = findChunk(fileStream, RIFF);

        Fourcc fileType;
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

    Handle EManager::Play(const Source& sound)
    {
        if (_xAudio2 == nullptr)
            throw AudioException("Audio manager is not initialized.");

        constexpr ThrowIfFailed throwIfFailed;

        // 초기화
        const WaveFormatHash hash        = GetWaveFormatHash(sound._format);
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
        unusedVoice.Callback     = IncreaseGenerationCallback(this, handle);

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

    void EManager::Stop(const Handle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to Stop.");

        constexpr ThrowIfFailed throwIfFailed;

        const SourceVoice& sourceVoice = _sourceVoices.at(handle._hash).at(handle._index);
        throwIfFailed(sourceVoice.Voice->Stop(0), "Failed to stop source voice.");

        ReleaseVoice(handle);
    }

    bool EManager::IsValidHandle(const Handle& handle) const noexcept
    {
        return _sourceVoices.contains(handle._hash) && handle._index >= 0 && handle._index < _sourceVoices.size() &&
               handle._generation == _sourceVoices.at(handle._hash).at(handle._index).Generation;
    }

    WaveFormatHash EManager::GetWaveFormatHash(const WAVEFORMATEX& waveFormat) const
    {
        WaveFormatHash seed        = 0;
        auto           hashCombine = [&seed]<typename T>(T value) {
            seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };
        hashCombine(waveFormat.wFormatTag);
        hashCombine(waveFormat.nChannels);
        hashCombine(waveFormat.nSamplesPerSec);
        hashCombine(waveFormat.nAvgBytesPerSec);
        hashCombine(waveFormat.nBlockAlign);
        hashCombine(waveFormat.wBitsPerSample);
        hashCombine(waveFormat.cbSize);

        return seed;
    }

    void EManager::ReleaseVoice(const Handle& handle)
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