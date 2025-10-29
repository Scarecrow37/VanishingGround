#include "pchScripts.h"
#include "FadeImageElement.h"

UMREAL_COMPONENT(FadeImageElement)

FadeImageElement::FadeImageElement() : UIAnimation([this](const float alpha) { UpdateAlpha(alpha); }), _fadeDirection(FadeDirection::NONE)
{
}

void FadeImageElement::FadeIn()
{
    _fadeDirection = FadeDirection::FORWARD;
    UIAnimation::Reset();
}

void FadeImageElement::FadeOut()
{
    _fadeDirection = FadeDirection::BACKWARD;
    UIAnimation::Reset();
}

void FadeImageElement::Stop()
{
    _fadeDirection = FadeDirection::NONE;
}

void FadeImageElement::Begin()
{
    SetElapsedTime(0.0f);
}

void FadeImageElement::End()
{
    const float duration = FadeDuration;
    SetElapsedTime(duration);
}

void FadeImageElement::Start()
{
    ImageElement::Start();

    UpdateAnimationProperty();
}

void FadeImageElement::Update()
{
    ImageElement::Update();

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

void FadeImageElement::Reset()
{
    ImageElement::Reset();

    _fadeDirection = FadeDirection::NONE;
}

void FadeImageElement::UpdateAlpha(const float alpha)
{
    Alpha = std::lerp(BeginAlpha, EndAlpha, alpha);
}

void FadeImageElement::UpdateAnimationProperty()
{
    const float duration = FadeDuration;
    SetDuration(duration);
}