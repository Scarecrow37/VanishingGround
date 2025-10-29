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
    UIAnimation::Reset();
}

void FadeDescriptionPanel::FadeOut()
{
    _fadeDirection = FadeDirection::BACKWARD;
    UIAnimation::Reset();
}

void FadeDescriptionPanel::Stop()
{
    _fadeDirection = FadeDirection::NONE;
}

void FadeDescriptionPanel::Begin()
{
    SetElapsedTime(0.0f);
}

void FadeDescriptionPanel::End()
{
    const float duration = FadeDuration;
    SetElapsedTime(duration);
}

void FadeDescriptionPanel::Start()
{
    DescriptionPanel::Start();

    UpdateAnimationProperty();
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

void FadeDescriptionPanel::UpdateAnimationProperty()
{
    const float duration = FadeDuration;
    SetDuration(duration);
}