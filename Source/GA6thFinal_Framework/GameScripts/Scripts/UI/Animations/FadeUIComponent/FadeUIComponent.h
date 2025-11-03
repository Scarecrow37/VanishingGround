#pragma once
#include "UI/Animations/UIAnimation.h"

class IOpacity;

class FadeUIComponent : public Component, public UIAnimation
{
    USING_PROPERTY(FadeUIComponent)

    enum class FadeDirection : unsigned char
    {
        NONE,
        FORWARD,
        BACKWARD,
    };

public:
    FadeUIComponent();

public:
    REFLECT_PROPERTY(FadeDuration, BeginOpacity, EndOpacity)

    GETTER(float, FadeDuration) { return ReflectFields->FadeDuration; }
    SETTER(float, FadeDuration)
    {
        ReflectFields->FadeDuration = std::max(0.1f, value);
        UpdateAnimationProperty();
    }
    PROPERTY(FadeDuration)

    GETTER(float, BeginOpacity) { return ReflectFields->BeginOpacity; }
    SETTER(float, BeginOpacity) { ReflectFields->BeginOpacity = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(BeginOpacity)

    GETTER(float, EndOpacity) { return ReflectFields->EndOpacity; }
    SETTER(float, EndOpacity) { ReflectFields->EndOpacity = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(EndOpacity)

public:
    void FadeIn();
    void FadeOut();
    void Stop();
    void Begin();
    void End();
    bool IsComplete() const;
    void CompleteImmediately();

protected:
    void Added() override;
    void Update() override;
    void Reset() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void UpdateOpacity(float value);
    void ClearTargets();
    void FindTargets();
    void UpdateAnimationProperty();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float FadeDuration = 1.0f;
    float BeginOpacity = 0.0f;
    float EndOpacity   = 1.0f;
    REFLECT_FIELDS_END(FadeUIComponent)

private:
    FadeDirection                        _fadeDirection;
    std::vector<std::weak_ptr<IOpacity>> _targets;
};
