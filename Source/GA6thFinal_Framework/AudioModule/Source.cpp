#include "pch.h"
#include "Source.h"

namespace Audio
{
    Source::Source(const WAVEFORMATEXTENSIBLE& format, const XAUDIO2_BUFFER& buffer) : _format(format), _buffer(buffer)
    {
    }

    Source::~Source()
    {
        delete[] _buffer.pAudioData;
    }

    Source::Source(Source&& other) noexcept : _format(other._format), _buffer(other._buffer)
    {
        other._buffer.pAudioData = nullptr;
    }

    Source& Source::operator=(Source&& other) noexcept
    {
        if (this == &other)
            return *this;
        _format                  = other._format;
        _buffer                  = other._buffer;
        other._buffer.pAudioData = nullptr; // Prevent double deletion
        return *this;
    }
} // namespace Audio