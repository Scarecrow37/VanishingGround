#include "pch.h"
#include "GroupHandle.h"

namespace Audio
{
    GroupHandle::GroupHandle() = default;
    GroupHandle::GroupHandle(const Index index, const Generation generation) : Handle(index, generation) {}

    bool GroupHandle::operator==(const GroupHandle& other) const noexcept
    {
        return Handle::operator==(other);
    }
} // namespace Audio