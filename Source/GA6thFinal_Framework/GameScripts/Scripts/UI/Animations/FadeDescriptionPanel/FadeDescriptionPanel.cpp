#include "pchScripts.h"
#include "FadeDescriptionPanel.h"

UMREAL_COMPONENT(FadeDescriptionPanel)

FadeDescriptionPanel::FadeDescriptionPanel()
    : UIAnimation([this](const float alpha) { UpdateAlpha(alpha); }), _isFading(false)
{
}

void FadeDescriptionPanel::Start()
{
    DescriptionPanel::Start();

    Alpha = 0.0f;
    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeDescriptionPanel::Update()
{
    DescriptionPanel::Update();

    if (_isFading)
        UIAnimation::Update(UmTime.DeltaTime());
}

void FadeDescriptionPanel::Reset()
{
    DescriptionPanel::Reset();

    _isFading = false;
}

void FadeDescriptionPanel::StartFade()
{
    _isFading = true;
}

void FadeDescriptionPanel::UpdateAlpha(const float alpha)
{
    Alpha = alpha;
}