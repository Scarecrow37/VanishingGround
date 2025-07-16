#include "pch.h"
#include "Handle.h"

namespace Audio
{
    Handle::Handle() : _hash(INVALID_WAVE_FORMAT_HASH), _index(INVALID_INDEX), _generation(INVALID_GENERATION) {}

    Handle::Handle(const WaveFormatHash hash, const Index index, const Generation generation)
        : _hash(hash), _index(index), _generation(generation)
    {
    }
} // namespace Audio