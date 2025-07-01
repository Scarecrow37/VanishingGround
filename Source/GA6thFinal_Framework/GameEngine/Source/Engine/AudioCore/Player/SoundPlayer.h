#pragma once

namespace Audio
{
    class SoundPlayer
    {
    public:
        SoundPlayer(IXAudio2SourceVoice* voice, const std::shared_ptr<const Sound>& sound);
        SoundPlayer(const SoundPlayer&) = delete;
        SoundPlayer& operator=(const SoundPlayer&) = delete;
        SoundPlayer(SoundPlayer&& other) noexcept;
        SoundPlayer& operator=(SoundPlayer&& other) noexcept;
        ~SoundPlayer();

        void Play() const;
        void Stop() const;

        void Resume() const;
        void Pause() const;

    private:
        void Submit() const;

        IXAudio2SourceVoice* _sourceVoice = nullptr;
        std::shared_ptr<const Sound> _sound;
    };
} // namespace Audio