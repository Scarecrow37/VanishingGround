#pragma once
#include <string>
#include <functional>

enum AnimationFlag
{
    ANIMATION_FLAG_NONE             = 0,
    ANIMATION_FLAG_USE_BLEND        = 1 << 0, // 애니메이션 블렌딩 활성화
    ANIMATION_FLAG_USE_LOOP         = 1 << 1, // 루프 애니메이션
    ANIMATION_FLAG_PAUSE            = 1 << 2, // 애니메이션 정지
    ANIMATION_FLAG_ALWAYS_UPDATE    = 1 << 3, // 애니메이션 활성화 중이 아니어도 항상 프레임 업데이트
    ANIMATION_FLAG_RESET_FRAME      = 1 << 4, // 애니메이션이 바뀔 때 프레임을 0으로 초기화
};
using AnimationFlags = int;

class AnimationData
{
    friend class AnimationComponent;

public:
    AnimationData(std::string_view key);
    AnimationData();
    ~AnimationData();

public:
    inline const std::string& GetAnimationName() const { return _animationName; }
    inline float GetAnimationSpeed() const { return _speed; }
    inline float GetAnimationElapsedFrame() const { return _elapsedFrame; }
    inline float GetAnimationMaxFrame() const { return _maxFrame; }
    inline bool  HasFlag(AnimationFlags flag) const { return _flag & flag; }

    inline bool  IsEnd() const { return _elapsedFrame >= _maxFrame; }
    inline bool  IsSameAnimation(std::string_view animName) const { return _animationName == animName; }
    inline bool  IsSameData(const AnimationData& animData) const { return _id == animData._id; }
    inline bool  IsSameID(UINT id) const { return _id == id; }

private:
    inline static UINT _nextID = 0;

    UINT            _id                 = ++_nextID;   // Data 식별용 ID
    std::string     _animationName      = "";          // 애니메이션 이름
    int             _flag               = ANIMATION_FLAG_NONE; // 애니메이션 옵션
    float           _speed              = 1.0f;        // 애니메이션 속도 (배수)
    float           _elapsedFrame       = 0.0f;        // 애니메이션 지속 시간 (초)
    float           _maxFrame           = 0.0f;        // 애니메이션 최대 프레임 (초 단위로 변환된 값)

    std::function<bool(const AnimationData&)> _popCondition = nullptr;      // return true일 시 Pop
    std::function<void()>                     _onPopCallback = nullptr;     // Pop 시 호출할 콜백 함수
    std::function<void()>                     _onEnterCallback = nullptr;   // Pop 시 호출할 콜백 함수
    std::function<void()>                     _onExitCallback  = nullptr;   // End 시 호출할 콜백 함수
    std::function<void()>                     _onEndCallback = nullptr;     // End 시 호출할 콜백 함수

    inline void SetFlag(AnimationFlags flag) { _flag = flag; }
    inline void AddFlag(AnimationFlags flag) { _flag |= flag; }
    inline void RemoveFlag(AnimationFlags flag) { _flag &= ~flag; }
    inline void ToggleFlag(AnimationFlags flag) { _flag ^= flag; }
};