#include "pchScripts.h"
#include "FadeImageElement.h"

#include "UI/Animations/FadeTextElement/FadeTextElement.h"

UMREAL_COMPONENT(FadeImageElement)

FadeImageElement::FadeImageElement() : UIAnimation([this](const float alpha) { UpdateAlpha(alpha); }), _fadeDirection(FadeDirection::NONE)
{
}

void FadeImageElement::FadeIn()
{
    _fadeDirection = FadeDirection::FORWARD;
}

void FadeImageElement::FadeOut()
{
    _fadeDirection = FadeDirection::BACKWARD;
}

void FadeImageElement::Stop()
{
    _fadeDirection = FadeDirection::NONE;
}

void FadeImageElement::Start()
{
    ImageElement::Start();

    Alpha = BeginAlpha;

    UIAnimation::Reset(ReflectFields->FadeDuration, false);
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