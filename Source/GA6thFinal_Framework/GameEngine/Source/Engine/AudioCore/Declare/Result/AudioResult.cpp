#include "pch.h"
#include "AudioResult.h"

namespace Audio
{
    Result HresultToAudioResult::operator()(const HRESULT resultHandle) const
    {
        switch (resultHandle)
        {
        case S_OK:
            return AUDIO_ERROR_SUCCESS;
        case XAUDIO2_E_INVALID_CALL:
            return AUDIO_ERROR_INVALID_CALL;
        case XAUDIO2_E_XMA_DECODER_ERROR:
            return AUDIO_ERROR_FAIL;
        case XAUDIO2_E_XAPO_CREATION_FAILED:
            return AUDIO_ERROR_FAIL;
        case XAUDIO2_E_DEVICE_INVALIDATED:
            return AUDIO_ERROR_DEVICE_INVALIDATED;
        default:
            return AUDIO_ERROR_UNKNOWN;
        }
    }
} // namespace Audio
