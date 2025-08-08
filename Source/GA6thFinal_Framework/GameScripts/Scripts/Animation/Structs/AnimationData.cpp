#include "pchScripts.h"
#include "AnimationData.h"

AnimationData::AnimationData(std::string_view key) : _animationName(key)
{
}

AnimationData::AnimationData() = default;

AnimationData::~AnimationData() = default;