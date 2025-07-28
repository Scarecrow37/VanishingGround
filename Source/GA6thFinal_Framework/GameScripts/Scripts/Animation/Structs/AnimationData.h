#pragma once
#include <string>
#include <functional>

enum AnimationFlag
{
    ANIMATION_FLAG_NONE             = 0,
    ANIMATION_FLAG_USE_LOOP         = 1 << 0, // 루프 애니메이션
    ANIMATION_FLAG_PAUSE            = 1 << 1, // 애니메이션 정지
    ANIMATION_FLAG_ALWAYS_UPDATE    = 1 << 2, // 항상 업데이트 애니메이션(OverrideAnimation은 마지막 애니메이션만 업데이트 하지만, 해당 bool값을 true로 할 경우 해당 애니메이션도 업데이트)
    ANIMATION_FLAG_RESET_FRAME      = 1 << 3, // 애니메이션이 바뀔 때 프레임을 0으로 초기화
};
using AnimationFlags = int;

class AnimationData
{
    friend class AnimationComponent;

public:
    AnimationData(std::string_view key)
        : AnimationName(key) {}
    AnimationData();
    ~AnimationData();

public:
    inline const std::string& GetAnimationName() const { return AnimationName; }
    inline bool  IsAnimationBlend() const { return IsBlending; }
    inline float GetAnimationSpeed() const { return Speed; }
    inline float GetAnimationElapsedFrame() const { return ElapsedFrame; }
    inline float GetAnimationMaxFrame() const { return MaxFrame; }
    inline bool  HasFlag(AnimationFlags flag) const { return Flags & flag; }

    inline bool  IsEnd() const { return ElapsedFrame >= MaxFrame; }
    inline bool  IsSameAnimation(const char* animName) const { return AnimationName == animName; }
    inline bool  IsSameData(const AnimationData& animData) const { return ID == animData.ID; }
    inline bool  IsSameID(UINT id) const { return ID == id; }

private:
    inline static UINT NextID = 0;

    UINT            ID              = ++NextID; // Data 식별용 ID
    std::string     AnimationName   = "";       // 애니메이션 이름
    AnimationFlags  Flags           = 0;        // 애니메이션 플래그
    bool            IsBlending      = false;    // 애니메이션 블렌딩 여부
    float           Speed           = 1.0f;     // 애니메이션 속도 (배수)
    float           ElapsedFrame    = 0.0f;     // 애니메이션 지속 시간 (초)
    float           MaxFrame        = 0.0f;     // 애니메이션 최대 프레임 (초 단위로 변환된 값)

    std::function<bool(const AnimationData&)> PopCondition = nullptr; // return true일 시 Pop

    inline void SetFlag(AnimationFlags flag) { Flags = flag; }
    inline void AddFlag(AnimationFlags flag) { Flags |= flag; }
    inline void RemoveFlag(AnimationFlags flag) { Flags &= ~flag; }
    inline void ToggleFlag(AnimationFlags flag) { Flags ^= flag; }
};