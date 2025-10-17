#include "pchScripts.h"
#include "FadeTextElement.h"

UMREAL_COMPONENT(FadeTextElement)

FadeTextElement::FadeTextElement() : UIAnimation([this](const float alpha) { UpdateAlpha(alpha); }), _isFading(false) {}

void FadeTextElement::StartFade()
{
    _isFading = true;
}

void FadeTextElement::Start()
{
    TextElement::Start();

    SimpleMath::Color color = Color;
    color.w                          = 0.0f;
    Color                            = color;

    UIAnimation::Reset(ReflectFields->FadeDuration, false);
}

void FadeTextElement::Update()
{
    TextElement::Update();

    if (_isFading)
        UIAnimation::Update(UmTime.DeltaTime());
}

void FadeTextElement::Reset()
{
    TextElement::Reset();

    _isFading = false;
}

void FadeTextElement::UpdateAlpha(const float alpha)
{
    DirectX::SimpleMath::Color color = Color;
    color.w                          = alpha;
    Color                            = color;
}