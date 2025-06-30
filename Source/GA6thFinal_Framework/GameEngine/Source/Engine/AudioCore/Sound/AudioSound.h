#pragma once

namespace Audio
{
    class Sound
    {
        friend class EManager;

        Sound(const WAVEFORMATEXTENSIBLE& format, const XAUDIO2_BUFFER& buffer);
    public:
        ~Sound();

    private:
        WAVEFORMATEXTENSIBLE _format;
        XAUDIO2_BUFFER       _buffer;
    };
}