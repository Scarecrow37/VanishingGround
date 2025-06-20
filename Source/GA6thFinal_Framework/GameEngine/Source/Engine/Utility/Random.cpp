#include "pch.h"

std::mt19937 Random::engine{std::random_device()()};

void Random::SetSeed(unsigned int seed)
{
    engine.seed(seed);
}
