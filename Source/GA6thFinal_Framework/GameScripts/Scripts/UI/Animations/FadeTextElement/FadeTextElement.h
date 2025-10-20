#pragma once
#include "UI/Elements/Text/TextElement.h"
#include "UI/Animations/UIAnimation.h"

class FadeTextElement : public TextElement, public UIAnimation
{
    USING_PROPERTY(FadeTextElement)

    enum class FadeDirection : unsigned char
    {
        NONE,
        FORWARD,
        BACKWARD,
    };

public:
    FadeTextElement();

public:
    REFLECT_PROPERTY(FadeDuration, BeginAlpha, EndAlpha)

    GETTER(float, FadeDuration) { return ReflectFields->FadeDuration; }
    SETTER(float, FadeDuration) { ReflectFields->FadeDuration = std::max(0.1f, value); }
    PROPERTY(FadeDuration)

    GETTER(float, BeginAlpha) { return ReflectFields->BeginAlpha; }
    SETTER(float, BeginAlpha) { ReflectFields->BeginAlpha = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(BeginAlpha)

    GETTER(float, EndAlpha) { return ReflectFields->EndAlpha; }
    SETTER(float, EndAlpha) { ReflectFields->EndAlpha = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(EndAlpha)

public:
    void FadeIn();
    void FadeOut();
    void Stop();
    void Begin();
    void End();

protected:
    void Start() override;

    void Update() override;

    void Reset() override;

private:
    void UpdateAlpha(float alpha);

protected:
    REFLECT_FIELDS_BEGIN(TextElement)
    float FadeDuration = 1.0f;
    float BeginAlpha   = 0.0f;
    float EndAlpha     = 1.0f;
    REFLECT_FIELDS_END(FadeTextElement)

private:
    FadeDirection _fadeDirection;
};