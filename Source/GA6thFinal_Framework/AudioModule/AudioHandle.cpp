#include "pch.h"
#include "AudioHandle.h"

namespace Audio
{
    AudioHandle::AudioHandle() : _hash(INVALID_WAVE_FORMAT_HASH) {}

    AudioHandle::AudioHandle(const WaveFormatHash hash, const Index index, const Generation generation)
        : Handle(index, generation), _hash(hash)
    {
    }
} // namespace Audio