#include "pch.h"
#include "AudioManager.h"
#include "Engine/AudioCore/Declare/RIFF/AudioChunk.h"
#include "Engine/AudioCore/Player/SoundPlayer.h"
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

    std::shared_ptr<Sound> EManager::CreateSoundFromWave(const std::filesystem::path& filePath)
    {
        std::ifstream fileStream;
        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fileStream.open(filePath, std::ios::binary);

        constexpr FindChunk     findChunk;
        constexpr ReadChunkData readChunkData;

        // Find the RIFF chunk
        auto [riffSize, riffPosition] = findChunk(fileStream, RIFF);

        Fourcc fileType;
        readChunkData(fileStream, &fileType, riffSize, riffPosition);

        if (fileType != WAVE)
            throw AudioException("Invalid file format: Not a WAVE file");

        // Find the fmt chunk
        auto [fmtSize, fmtPosition] = findChunk(fileStream, FMT);

        WAVEFORMATEXTENSIBLE wfx{};
        readChunkData(fileStream, &wfx, fmtSize, fmtPosition);

        // Find the data chunk
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
        buffer.AudioBytes = static_cast<UINT32>(dataSize);
        buffer.pAudioData = audioData;
        buffer.Flags      = XAUDIO2_END_OF_STREAM; // Indicates the end of the stream

        return std::make_shared<Sound>(wfx, buffer);
    }

    std::shared_ptr<SoundPlayer> EManager::CreatePlayer(const std::shared_ptr<Sound>& sound) const
    {
        if (_xAudio2 == nullptr)
            throw AudioException("Audio manager is not initialized.");

        constexpr ThrowIfFailed throwIfFailed;

        IXAudio2SourceVoice* sourceVoice;
        throwIfFailed(_xAudio2->CreateSourceVoice(&sourceVoice, reinterpret_cast<const WAVEFORMATEX*>(&sound->_format)),
                      "Failed to create source voice.");

        return std::make_shared<SoundPlayer>(sourceVoice, sound);
    }
} // namespace Audio