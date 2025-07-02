#pragma once

namespace Audio
{
    using WaveFormatHash                              = unsigned long long;
    constexpr WaveFormatHash INVALID_WAVE_FORMAT_HASH = 0;

    using Index                   = long long;
    constexpr Index INVALID_INDEX = -1;

    using Generation                        = long long;
    constexpr Generation INVALID_GENERATION = -1;

    struct IncreaseGeneration
    {
        Generation& operator()(Generation& generation) const;
    };

    struct IsUnusedGeneration
    {
        bool operator()(const Generation& generation) const;
    };
} // namespace Audio