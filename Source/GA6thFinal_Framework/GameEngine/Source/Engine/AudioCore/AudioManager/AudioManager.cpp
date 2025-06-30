#include "pch.h"
#include "AudioManager.h"

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
} // namespace Audio