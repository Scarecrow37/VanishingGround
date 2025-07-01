#pragma once

namespace Audio
{
    class SoundPlayer
    {
    public:
        SoundPlayer();
        SoundPlayer(const SoundPlayer&) = delete;
        SoundPlayer& operator=(const SoundPlayer&) = delete;
        SoundPlayer(SoundPlayer&& other) noexcept;
        SoundPlayer& operator=(SoundPlayer&& other) noexcept;
        ~SoundPlayer();
    private:
        IXAudio2SourceVoice* _sourceVoice = nullptr;
    };
} // namespace Audio