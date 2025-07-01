#include "pch.h"
#include "AudioSound.h"

namespace Audio
{
    Sound::Sound(const WAVEFORMATEXTENSIBLE& format, const XAUDIO2_BUFFER& buffer) : _format(format), _buffer(buffer) {}

    Sound::~Sound()
    {
        delete[] _buffer.pAudioData;
    }

    Sound::Sound(Sound&& other) noexcept : _format(other._format), _buffer(other._buffer)
    {
        other._buffer.pAudioData = nullptr;
    }

    Sound& Sound::operator=(Sound&& other) noexcept
    {
        if (this == &other)
            return *this;
        _format                  = other._format;
        _buffer                  = other._buffer;
        other._buffer.pAudioData = nullptr; // Prevent double deletion
        return *this;
    }
} // namespace Audio