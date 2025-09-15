#pragma once
#include "UmFramework.h"
class SceneTransitionComponent : public Component
{
public:
    SceneTransitionComponent();
    ~SceneTransitionComponent() override;

    USING_PROPERTY(SceneTransitionComponent)
public:
    REFLECT_PROPERTY(Duration, Maintain, StartColor, EndColor, Easing)

    GETTER(float, Duration) { return ReflectFields->Duration; }
    SETTER(float, Duration) { ReflectFields->Duration = value; }
    PROPERTY(Duration)

    GETTER(float, Maintain) { return ReflectFields->Maintain; }
    SETTER(float, Maintain) { ReflectFields->Maintain = value; }
    PROPERTY(Maintain)

    GETTER(Color, StartColor) { return _startColor; }
    SETTER(Color, StartColor) { _startColor = value; }
    PROPERTY(StartColor)

    GETTER(Color, EndColor) { return _endColor; }
    SETTER(Color, EndColor) { _endColor = value; }
    PROPERTY(EndColor)

    GETTER(bool, Easing) { return ReflectFields->Ease; }
    SETTER(bool, Easing) { ReflectFields->Ease = value; }
    PROPERTY(Easing)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::array<float, 4> StartColorArray;
    std::array<float, 4> EndColorArray;
    float                Duration;
    float                Maintain;
    bool                 Ease;
    UINT                 EaseType = 0;
    UINT                 EaseFuncType   = 0;
    float                EaseThreshold = 0.5f;
    REFLECT_FIELDS_END(SceneTransitionComponent)

    Color              _startColor;
    Color              _endColor;
    float              _fadeElapsedTimer = 0.f;
    bool               _fadeFlag         = false;
    std::vector<float> _easeLog;


    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;
    void Update() override;

    void CalculateFade();

public:
    void Fade(float duration, float maintain, const Vector4& start, const Vector4& end);
};
