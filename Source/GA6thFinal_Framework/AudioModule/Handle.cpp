#include "pch.h"
#include "Handle.h"

namespace Audio
{
    Handle::Handle() : _index(INVALID_INDEX), _generation(INVALID_GENERATION) {}

    Handle::Handle(const Index index, const Generation generation) : _index(index), _generation(generation) {}

    bool Handle::operator==(const Handle& other) const noexcept
    {
        return _index == other._index && _generation == other._generation;
    }
} // namespace Audio