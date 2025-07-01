#pragma once

namespace Audio
{
    class Sound
    {
        friend class EManager;

    public:
        Sound(const WAVEFORMATEX& format, const XAUDIO2_BUFFER& buffer);
        Sound(const Sound&)            = delete;
        Sound& operator=(const Sound&) = delete;
        Sound(Sound&& other) noexcept;
        Sound& operator=(Sound&& other) noexcept;
        ~Sound();

    private:
        WAVEFORMATEX _format;
        XAUDIO2_BUFFER       _buffer;
    };
} // namespace Audio