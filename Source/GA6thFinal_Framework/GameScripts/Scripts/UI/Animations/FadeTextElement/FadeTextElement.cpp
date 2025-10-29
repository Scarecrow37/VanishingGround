#include "pchScripts.h"
#include "FadeTextElement.h"

UMREAL_COMPONENT(FadeTextElement)

FadeTextElement::FadeTextElement() : UIAnimation([this](const float alpha) { UpdateAlpha(alpha); }), _fadeDirection(FadeDirection::NONE) {}

void FadeTextElement::FadeIn()
{
    _fadeDirection = FadeDirection::FORWARD;
    UIAnimation::Reset();
}

void FadeTextElement::FadeOut()
{
    _fadeDirection = FadeDirection::BACKWARD;
    UIAnimation::Reset();
}

void FadeTextElement::Stop()
{
    _fadeDirection = FadeDirection::NONE;
}

void FadeTextElement::Begin()
{
    SetElapsedTime(0.0f);
}

void FadeTextElement::End()
{
    const float duration = FadeDuration;
    SetElapsedTime(duration);
}

void FadeTextElement::Start()
{
    TextElement::Start();

    UpdateAnimationProperty();
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
    const float opacity = std::lerp(BeginAlpha, EndAlpha, alpha);
    SetOpacity(opacity);
}

void FadeTextElement::UpdateAnimationProperty()
{
    const float duration = FadeDuration;
    SetDuration(duration);
}