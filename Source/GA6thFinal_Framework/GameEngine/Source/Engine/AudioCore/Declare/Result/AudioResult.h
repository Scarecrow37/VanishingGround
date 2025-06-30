#pragma once

namespace Audio
{
    using Result = unsigned char;

    constexpr Result AUDIO_ERROR_SUCCESS              = 0;
    constexpr Result AUDIO_ERROR_FAIL                 = 1;
    constexpr Result AUDIO_ERROR_INVALID_CALL         = 2;
    constexpr Result AUDIO_ERROR_DEVICE_INVALIDATED   = 3;
    constexpr Result AUDIO_ERROR_FILE_NOT_FOUND       = 4;
    constexpr Result AUDIO_ERROR_INVALID_FILE_POINTER = 5;
    constexpr Result AUDIO_ERROR_FILE_READ_FAIL       = 6;
    constexpr Result AUDIO_ERROR_INVALID_FILE_FORMAT  = 7;
    constexpr Result AUDIO_ERROR_UNKNOWN              = 255;

    struct HresultToAudioResult
    {
        Result operator()(HRESULT resultHandle) const;
    };
} // namespace Audio