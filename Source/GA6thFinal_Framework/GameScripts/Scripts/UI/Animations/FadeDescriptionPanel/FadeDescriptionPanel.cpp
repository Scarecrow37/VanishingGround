#include "pchScripts.h"
#include "FadeDescriptionPanel.h"

UMREAL_COMPONENT(FadeDescriptionPanel)

FadeDescriptionPanel::FadeDescriptionPanel()
    : UIAnimation([this](const float alpha) { UpdateAlpha(alpha); }), _fadeDirection(FadeDirection::NONE)
{
}

void FadeDescriptionPanel::FadeIn()
{
    _fadeDirection = FadeDirection::FORWARD;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeDescriptionPanel::FadeOut()
{
    _fadeDirection = FadeDirection::BACKWARD;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeDescriptionPanel::Stop()
{
    _fadeDirection = FadeDirection::NONE;
}

void FadeDescriptionPanel::Begin()
{
    Alpha = BeginAlpha;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeDescriptionPanel::End()
{
    Alpha = EndAlpha;
    UIAnimation::Reset(0.0f, false);
}

void FadeDescriptionPanel::Start()
{
    DescriptionPanel::Start();

    Alpha = BeginAlpha;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeDescriptionPanel::Update()
{
    DescriptionPanel::Update();

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

void FadeDescriptionPanel::Reset()
{
    DescriptionPanel::Reset();

    _fadeDirection = FadeDirection::NONE;
}

void FadeDescriptionPanel::UpdateAlpha(const float alpha)
{
    Alpha = std::lerp(BeginAlpha, EndAlpha, alpha);
}