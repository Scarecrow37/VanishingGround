#include "pch.h"
#include "EffectHandle.h"

namespace Audio
{
    EffectHandle::EffectHandle() = default;
    EffectHandle::EffectHandle(const Index index, const Generation generation) : Handle(index, generation) {}
} // namespace Audio