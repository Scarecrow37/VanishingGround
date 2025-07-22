#pragma once
#include <string>
#include <functional>

struct AnimationData
{
    AnimationData(std::string_view key, bool loop)
        : AnimationName(key), IsLooping(loop), IsPlaying(true), IsEnd(false), Speed(1.0f), Duration(0.0f)
    {}
    AnimationData() = default;
    ~AnimationData() = default;

    std::string AnimationName = "";    // 애니메이션 이름
    bool        IsLooping     = false; // 애니메이션 루프 여부
    bool        IsPlaying     = true;  // 애니메이션 재생 여부
    bool        IsEnd         = false; // 애니메이션 종료 여부
    float       Speed         = 1.0f;  // 애니메이션 속도 (배수)
    float       Duration      = 0.0f;  // 애니메이션 지속 시간 (초)

    std::function<bool(const AnimationData&)> PopCondition = nullptr; // return true일 시 Pop
};