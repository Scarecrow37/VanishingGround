#include "pch.h"
#include "ReverbHandle.h"

namespace Audio
{
    ReverbHandle::ReverbHandle() = default;
    ReverbHandle::ReverbHandle(const Index index, const Generation generation) : EffectHandle(index, generation) {}
} // namespace Audio