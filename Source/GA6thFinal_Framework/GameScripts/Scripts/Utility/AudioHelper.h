#pragma once
#include <string>

namespace AudioHelper
{
    inline static bool PlaySFX(const std::string& bgmKey)
    {
        if ('0' != bgmKey[0])
        {
            UmAudio.Play(bgmKey);
            return true;
        }
        return false;
    }
    inline static bool PlaySFX(int id)
    {
        if (0 != id)
        {
            return PlaySFX(std::to_string(id));
        }
        return false;
    }
    inline static bool PlayBGM(const std::string& bgmKey, bool isLoop = true)
    {
        if ('0' != bgmKey[0])
        {
            UmAudio.Play(bgmKey, Audio::GROUP_BGM, isLoop);
            return true;
        }
        return false;
    }
    inline static bool PlayBGM(int id, bool isLoop = true)
    {
        if (0 != id)
        {
            return PlayBGM(std::to_string(id), isLoop);
        }
        return false;
    }
}