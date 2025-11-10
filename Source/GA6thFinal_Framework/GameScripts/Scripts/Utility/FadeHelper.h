#pragma once
#include "UmFramework.h"

class Fader
{
public:
    enum Mode
    {
        FADE_NONE,
        FADE_IN,
        FADE_OUT
    };

    Fader()  = default;
    ~Fader() = default;

public:
    /// <summary>모드에 따라 페이드를 갱신합니다. 0~1 사이의 Factor를 반환합니다.</summary>
    float Fade();
    /// <summary>페이드 인이 종료되었는지 반환합니다.</summary>
    inline bool IsFadeInEnd() const { return 0.9999f <= _fadeFactor; }
    /// <summary>페이드 아웃이 종료되었는지 반환합니다.</summary>
    inline bool IsFadeOutEnd() const { return 0.0001f >= _fadeFactor; }

public:
    /// <summary>페이드를 UnScaledDeltaTime을 쓸지 여부를 변경합니다.</summary>
    inline void UseUnScaledDeltaTime(bool useUnScaledTime) { _useUnscaledTime = useUnScaledTime; }

    /// <summary>페이드 인이 끝났을 때 호출될 콜백 함수를 설정합니다. 생명관리를 하지 않으므로 사용 시 주의해야 합니다.</summary>
    /// <param name="callback">페이드 인 종료 시 실행할 콜백 함수입니다.</param>
    inline void SetOnFadeInEndCallback(const std::function<void()>& callback) { _onFadeInEndCallback = callback; }

    /// <summary>페이드 아웃이 끝날 때 호출될 콜백 함수를 설정합니다. 생명관리를 하지 않으므로 사용 시 주의해야 합니다.</summary>
    /// <param name="callback">페이드 아웃 종료 시 실행할 콜백 함수입니다.</param>
    inline void SetOnFadeOutEndCallback(const std::function<void()>& callback) { _onFadeOutEndCallback = callback; }

    /// <summary>페이드 모드를 설정합니다.</summary>
    /// <param name="mode">설정할 페이드 모드입니다.</param>
    inline void SetFadeMode(Mode mode) { _fadeMode = mode; }

    /// <summary>지정된 지속 시간을 설정합니다.</summary>
    /// <param name="duration">설정할 지속 시간(초 단위)입니다.</param>
    inline void SetDuration(float duration) {
        _duration = std::max(duration, 0.0f);
        _fadeFactor = _timer / _duration;
    }

    /// <summary>타이머 값을 설정합니다. 0부터 duration 사이의 값을 클램핑합니다.</summary>
    /// <param name="timer">설정할 타이머 값(초 단위)입니다.</param>
    inline void SetTimer(float timer) {
        _timer = std::clamp(timer, 0.0f, _duration); 
        _fadeFactor = _timer / _duration;
    }

    /// <summary>타이머 값을 설정합니다. 0부터 1 사이의 값을 클램핑합니다.</summary>
    /// <param name="timer">설정할 타이머 값(초 단위)입니다.</param>
    inline void SetTimerFromFactor(float factor) { _timer = _duration * std::clamp(factor, 0.0f, 1.0f); }

    /// <summary>페이드 인 효과의 타입과 함수 유형을 설정합니다.</summary>
    /// <param name="type">적용할 페이드 인 이징 타입입니다.</param>
    /// <param name="func">사용할 이징 함수 유형입니다.</param>
    inline void SetFadeInType(Mathf::EaseType type, Mathf::EaseFuncType func)
    {
        _fadeInEaseType = type;
        _fadeInFuncType = func;
    }

    /// <summary>페이드 아웃 효과의 타입과 함수 유형을 설정합니다.</summary>
    /// <param name="type">적용할 페이드 아웃 이징 타입입니다.</param>
    /// <param name="func">적용할 이징 함수 유형입니다.</param>
    inline void SetFadeOutType(Mathf::EaseType type, Mathf::EaseFuncType func)
    {
        _fadeOutEaseType = type;
        _fadeOutFuncType = func;
    }

    inline void Reset()
    {
        if (FADE_IN == _fadeMode)
        {
            _timer = 0.0f;
            _fadeFactor = 0.0f;
        }
        else if (FADE_OUT == _fadeMode)
        {
            _timer      = _duration;
            _fadeFactor = 1.0f;
        }
    }

    inline Mode                GetFadeMode() const { return _fadeMode; }
    inline float               GetFadeFactor() const { return _fadeFactor; }
    inline float               GetDuration() const { return _duration; }
    inline Mathf::EaseType     GetFadeInEaseType() const { return _fadeInEaseType; }
    inline Mathf::EaseFuncType GetFadeInFuncType() const { return _fadeInFuncType; }
    inline Mathf::EaseType     GetFadeOutEaseType() const { return _fadeOutEaseType; }
    inline Mathf::EaseFuncType GetFadeOutFuncType() const { return _fadeOutFuncType; }

private:
    /// <summary>FadeIn을 갱신합니다. 0~1 사이의 Factor를 반환합니다.</summary>
    float FadeIn();
    /// <summary>FadeOut을 갱신합니다. 0~1 사이의 Factor를 반환합니다.</summary>
    float FadeOut();

private:
    bool  _useUnscaledTime  = false;
    float _duration         = 0.0f;
    float _timer            = 0.0f;
    float _fadeFactor       = 0.0f;
    Mode  _fadeMode         = FADE_NONE;

    Mathf::EaseType     _fadeInEaseType  = Mathf::EASE_IN;
    Mathf::EaseFuncType _fadeInFuncType  = Mathf::SINE;
    Mathf::EaseType     _fadeOutEaseType = Mathf::EASE_OUT;
    Mathf::EaseFuncType _fadeOutFuncType = Mathf::SINE;

    std::function<void()> _onFadeInEndCallback  = nullptr;
    std::function<void()> _onFadeOutEndCallback = nullptr;
};