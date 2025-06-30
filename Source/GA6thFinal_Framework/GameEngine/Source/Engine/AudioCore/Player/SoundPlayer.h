#pragma once

namespace Audio
{
    class SoundPlayer
    {
        friend class EManager;

        SoundPlayer();

    public:
        ~SoundPlayer();

    private:
        IXAudio2SourceVoice* _sourceVoice = nullptr;
    };
} // namespace Audio