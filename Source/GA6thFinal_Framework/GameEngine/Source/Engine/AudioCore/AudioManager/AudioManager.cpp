#include "pch.h"
#include "AudioManager.h"
#include "Engine/AudioCore/Declare/RIFF/AudioChunk.h"
#include "Engine/AudioCore/Sound/AudioSound.h"

namespace Audio
{
    EManager::EManager() = default;

    EManager::~EManager() = default;

    Result EManager::Initialize()
    {
        constexpr HresultToAudioResult hresultToAudioResult;
        HRESULT                        resultHandle = S_OK;

        resultHandle = XAudio2Create(_xAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(resultHandle))
            return hresultToAudioResult(resultHandle);

        resultHandle = _xAudio2->CreateMasteringVoice(&_masteringVoice);
        if (FAILED(resultHandle))
            return hresultToAudioResult(resultHandle);

        return AUDIO_ERROR_SUCCESS;
    }

    Result EManager::CreateSoundFromWave(const std::filesystem::path& filePath, Sound** sound)
    {
        constexpr HresultToAudioResult hresultToAudioResult;
        Result                         result = AUDIO_ERROR_SUCCESS;

        std::ifstream fileStream(filePath, std::ios::binary);
        if (!fileStream.is_open())
            return AUDIO_ERROR_FILE_NOT_FOUND;

        constexpr FindChunk     findChunk;
        constexpr ReadChunkData readChunkData;

        std::streamsize chunkSize;
        DWORD chunkPosition;

        // Find the RIFF chunk
        result = findChunk(fileStream, RIFF, chunkSize, chunkPosition);
        if (result != AUDIO_ERROR_SUCCESS)
            return result;

        Fourcc fileType;
        result = readChunkData(fileStream, &fileType, chunkSize, chunkPosition);
        if (result != AUDIO_ERROR_SUCCESS)
            return result;

        if (fileType != WAVE)
            return AUDIO_ERROR_INVALID_FILE_FORMAT;

        // Find the fmt chunk
        result = findChunk(fileStream, FMT, chunkSize, chunkPosition);
        if (result != AUDIO_ERROR_SUCCESS)
            return result;

        WAVEFORMATEXTENSIBLE wfx{};
        result = readChunkData(fileStream, &wfx, chunkSize, chunkPosition);
        if (result != AUDIO_ERROR_SUCCESS)
            return result;

        // Find the data chunk
        result = findChunk(fileStream, DATA, chunkSize, chunkPosition);
        if (result != AUDIO_ERROR_SUCCESS)
            return result;

        BYTE* audioData = new BYTE[chunkSize];
        result            = readChunkData(fileStream, audioData, chunkSize, chunkPosition);
        if (result != AUDIO_ERROR_SUCCESS)
        {
            delete[] audioData;
            return result;
        }

        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = chunkSize;
        buffer.pAudioData = audioData;
        buffer.Flags      = XAUDIO2_END_OF_STREAM; // Indicates the end of the stream

        *sound = new Sound(wfx, buffer);

        return AUDIO_ERROR_SUCCESS;
    }
} // namespace Audio