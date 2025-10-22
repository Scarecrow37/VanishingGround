#include "pchScripts.h"
#include "FadeTextElement.h"

UMREAL_COMPONENT(FadeTextElement)

FadeTextElement::FadeTextElement() : UIAnimation([this](const float alpha) { UpdateAlpha(alpha); }), _fadeDirection(FadeDirection::NONE) {}

void FadeTextElement::FadeIn()
{
    _fadeDirection = FadeDirection::FORWARD;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeTextElement::FadeOut()
{
    _fadeDirection = FadeDirection::BACKWARD;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeTextElement::Stop()
{
    _fadeDirection = FadeDirection::NONE;
}

void FadeTextElement::Begin()
{
    SimpleMath::Color color = Color;
    color.w                 = BeginAlpha;
    Color                   = color;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeTextElement::End()
{
    SimpleMath::Color color = Color;
    color.w                 = EndAlpha;
    Color                   = color;
    UIAnimation::Reset(0.0f, false);
}

void FadeTextElement::Start()
{
    TextElement::Start();

    SimpleMath::Color color = Color;
    color.w                          = BeginAlpha;
    Color                            = color;

    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeTextElement::Update()
{
    TextElement::Update();

    switch (_fadeDirection)
    {
    case FadeDirection::NONE:
        break;
    case FadeDirection::FORWARD:
        UIAnimation::Update(UmTime.DeltaTime());
        break;
    case FadeDirection::BACKWARD:
        UIAnimation::Update(-UmTime.DeltaTime());
        break;
    }
}

void FadeTextElement::Reset()
{
    TextElement::Reset();

    _fadeDirection = FadeDirection::NONE;
}

void FadeTextElement::UpdateAlpha(const float alpha)
{
    DirectX::SimpleMath::Color color = Color;
    color.w                          = std::lerp(BeginAlpha, EndAlpha, alpha);
    Color                            = color;
}