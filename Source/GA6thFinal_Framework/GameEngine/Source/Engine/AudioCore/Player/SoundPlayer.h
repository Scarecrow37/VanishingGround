#pragma once

namespace Audio
{
    class SoundPlayer
    {

    private:
        IXAudio2SourceVoice* _sourceVoice = nullptr;
    };
} // namespace Audio