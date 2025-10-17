#pragma once
#include "UI/Elements/Text/TextElement.h"
#include "UI/Animations/UIAnimation.h"

class FadeTextElement : public TextElement, public UIAnimation
{
    USING_PROPERTY(FadeTextElement)

public:
    FadeTextElement();

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
    REFLECT_FIELDS_BEGIN(TextElement)
    float FadeDuration = 1.0f;
    REFLECT_FIELDS_END(FadeTextElement)

private:
    bool _isFading;
};