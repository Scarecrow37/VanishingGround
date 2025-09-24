#include "pch.h"
#include "Source.h"

namespace Audio
{
    Source::~Source()
    {
        delete[] _buffer;
    }

    Source::Source(const WAVEFORMATEXTENSIBLE& format, const BYTE* buffer, const UINT32 bytes)
        : _format(format), _buffer(buffer), _bytes(bytes)
    {
    }

    Source::Source(Source&& other) noexcept : _format(other._format), _buffer(other._buffer), _bytes(other._bytes)
    {
        other._buffer = nullptr;
    }

    Source& Source::operator=(Source&& other) noexcept
    {
        if (this == &other)
            return *this;
        _format                  = other._format;
        _buffer                  = other._buffer;
        _bytes                   = other._bytes;
        other._buffer = nullptr; // Prevent double deletion
        return *this;
    }
} // namespace Audio