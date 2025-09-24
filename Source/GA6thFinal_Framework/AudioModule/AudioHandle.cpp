#include "pch.h"
#include "AudioHandle.h"

namespace Audio
{
    AudioHandle::AudioHandle() : _hash(INVALID_WAVE_FORMAT_HASH), _index(INVALID_INDEX), _generation(INVALID_GENERATION) {}

    AudioHandle::AudioHandle(const WaveFormatHash hash, const Index index, const Generation generation)
        : _hash(hash), _index(index), _generation(generation)
    {
    }
} // namespace Audio