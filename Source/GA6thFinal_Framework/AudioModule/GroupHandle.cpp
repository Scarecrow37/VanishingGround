#include "pch.h"
#include "GroupHandle.h"

namespace Audio
{
    GroupHandle::GroupHandle() = default;
    GroupHandle::GroupHandle(const Index index, const Generation generation) : Handle(index, generation) {}
} // namespace Audio