#pragma once
#include "UI/Animations/UIAnimation.h"
#include "UI/Panels/Description/DescriptionPanel.h"

class FadeDescriptionPanel : public DescriptionPanel, public UIAnimation
{
    USING_PROPERTY(FadeDescriptionPanel)

public:
    FadeDescriptionPanel();

public:
    REFLECT_PROPERTY(FadeDuration)

    GETTER(float, FadeDuration) { return ReflectFields->FadeDuration; }
    SETTER(float, FadeDuration) { ReflectFields->FadeDuration = std::max(0.1f, value); }
    PROPERTY(FadeDuration)

public:
    void StartFade();

protected:
    void Start() override;

    void Update() override;

    void Reset() override;

private:
    void UpdateAlpha(float alpha);

protected:
    REFLECT_FIELDS_BEGIN(DescriptionPanel)
    float FadeDuration = 1.0f;
    REFLECT_FIELDS_END(FadeDescriptionPanel)

private:
    bool _isFading;
};