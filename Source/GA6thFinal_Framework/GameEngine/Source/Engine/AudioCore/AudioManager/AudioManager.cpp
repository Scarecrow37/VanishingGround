#include "pch.h"
#include "AudioManager.h"
#include "Engine/AudioCore/Declare/RIFF/AudioChunk.h"
#include "Engine/AudioCore/Handle/AudioHandle.h"
#include "Engine/AudioCore/Sound/AudioSound.h"

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

    Sound EManager::CreateSoundFromWave(const std::filesystem::path& filePath, const bool isLoop)
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
            buffer.LoopBegin = 0; 
            buffer.LoopLength = 0;
            buffer.LoopCount  = XAUDIO2_LOOP_INFINITE;
        }

        return Sound{wfx, buffer};
    }

    Handle EManager::Play(const Sound& sound)
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
        std::vector<Generation>*           generations  = nullptr;
        std::vector<IXAudio2SourceVoice*>* sourceVoices = nullptr;
        if (_generationMap.contains(hash)) // 없으면 생성
        {
            _generationMap[hash] = std::vector<Generation>{};
            _sourceVoices[hash]  = std::vector<IXAudio2SourceVoice*>{};
        }
        generations  = &_generationMap[hash];
        sourceVoices = &_sourceVoices[hash];

        // 미사용 Voice 찾기
        auto unusedGenerationIterator = std::ranges::find_if(*generations, [](const Generation& gen) {
            static IsUnusedGeneration isUnusedGeneration;
            return isUnusedGeneration(gen);
        });

        if (unusedGenerationIterator == generations->end()) // 없으면 생성
        {
            IXAudio2SourceVoice* tempSourceVoice = nullptr;
            throwIfFailed(_xAudio2->CreateSourceVoice(&tempSourceVoice, &sound._format),
                          "Failed to create source voice.");
            sourceVoices->push_back(tempSourceVoice);

            generations->push_back(0);
            unusedGenerationIterator = std::prev(generations->end());
        }

        generation  = IncreaseGeneration()(*unusedGenerationIterator);
        index       = std::distance(generations->begin(), unusedGenerationIterator);
        sourceVoice = (*sourceVoices)[index];

        // Submit 후 시작
        throwIfFailed(sourceVoice->SubmitSourceBuffer(&sound._buffer), "Failed to submit source buffer.");
        throwIfFailed(sourceVoice->Start(0), "Failed to start source voice.");

        return Handle{hash, index, generation};
    }

    void EManager::Stop(const Handle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to Stop.");

        constexpr ThrowIfFailed throwIfFailed;
        constexpr IncreaseGeneration increaseGeneration;

        IXAudio2SourceVoice* sourceVoice = _sourceVoices[handle._hash][handle._index];
        throwIfFailed(sourceVoice->Stop(0), "Failed to stop source voice.");
        throwIfFailed(sourceVoice->FlushSourceBuffers(), "Failed to flush source buffers.");

        increaseGeneration(_generationMap[handle._hash][handle._index]);
    }

    bool EManager::IsValidHandle(const Handle& handle) const
    {
        return _generationMap.contains(handle._hash) && handle._index >= 0 && handle._index < _generationMap.size() &&
               handle._generation == _generationMap.at(handle._hash)[handle._index];
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
} // namespace Audio