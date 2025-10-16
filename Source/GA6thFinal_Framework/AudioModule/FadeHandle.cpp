#include "pch.h"
#include "FadeHandle.h"

namespace Audio
{
    FadeHandle::FadeHandle() = default;
    FadeHandle::FadeHandle(const Index index, const Generation generation) : EffectHandle(index, generation) {}
}