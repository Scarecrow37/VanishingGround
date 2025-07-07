#include "pch.h"
#include "Types.h"

namespace Audio
{
    Generation& IncreaseGeneration::operator()(Generation& generation) const
    {
        ++generation;
        return generation;
    }

    bool IsUnusedGeneration::operator()(const Generation& generation) const
    {
        return generation % 2 == 0;
    }
} // namespace Audio