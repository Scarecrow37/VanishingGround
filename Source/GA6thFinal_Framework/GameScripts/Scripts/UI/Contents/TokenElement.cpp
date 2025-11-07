#include "pchScripts.h"
#include "TokenElement.h"

#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(TokenElement)

TokenElement::TokenElement() = default;

void TokenElement::Setup(const SetupData& data)
{
    if (_iconImageElement.expired() || _nameTextElement.expired())
    {
        FindComponents();
    }

    _beginPoint   = data.BeginPoint;
    _endPoint     = data.EndPoint;
    _beginOpacity = data.BeginOpacity;
    _endOpacity   = data.EndOpacity;
    _duration     = std::max(data.Duration, MIN_DURATION);

    if (const auto sharedIconImageElement = _iconImageElement.lock())
    {
        sharedIconImageElement->SetImage(data.IconGuid);
    }

    if (const auto sharedNameTextElement = _nameTextElement.lock())
    {
        sharedNameTextElement->Text  = data.TokenName;
        sharedNameTextElement->Color = data.NameColor;
    }

    ResetState();
}

void TokenElement::Update()
{
    HorizontalPanel::Update();

    _elapsedTime += UmTime.DeltaTime();

    if (constexpr Mathf::CompareFloat compareFloat; compareFloat(_elapsedTime, _duration) == std::partial_ordering::greater)
    {
        gameObject->SetActive(false);
        return;
    }

    const float t = GetT();
    UpdatePoint(t);
    UpdateOpacity(t);
}

POINT TokenElement::GetPoint(const float t) const
{
    const LONG x = std::lerp(_beginPoint.x, _endPoint.x, t);
    const LONG y = std::lerp(_beginPoint.y, _endPoint.y, t);
    return POINT{.x = x, .y = y};
}

float TokenElement::GetOpacity(const float t) const
{
    return std::lerp(_beginOpacity, _endOpacity, t);
}

float TokenElement::GetT() const
{
    const float t = std::clamp(_elapsedTime / _duration, 0.0f, 1.0f);
    return Mathf::Ease(Mathf::EaseType::EASE_OUT, Mathf::SINE, 0.5f, t);
}

void TokenElement::UpdatePoint(const float t)
{
    Point = GetPoint(t);
}

void TokenElement::UpdateOpacity(const float t) const
{
    const float opacity = GetOpacity(t);

    if (const auto sharedIconImageElement = _iconImageElement.lock())
    {
        sharedIconImageElement->SetOpacity(opacity);
    }

    if (const auto sharedNameTextElement = _nameTextElement.lock())
    {
        sharedNameTextElement->SetOpacity(opacity);
    }
}

void TokenElement::FindComponents()
{
    Transform::ForeachBFS(transform, [this](const Transform* childTransform, const int depth) {
        if (depth == 1)
        {
            const GameObject& childGameObject = childTransform->gameObject;
            if (const ImageElement* imageElement = childGameObject.GetComponentDynamic<ImageElement>())
            {
                _iconImageElement = imageElement->GetWeakPtrAs<ImageElement>();
            }
            else if (const TextElement* textElement = childGameObject.GetComponentDynamic<TextElement>())
            {
                _nameTextElement = textElement->GetWeakPtrAs<TextElement>();
            }
        }
    });
}

void TokenElement::ResetState()
{
    _elapsedTime = 0.0f;

    UpdatePoint(0.0f);
    UpdateOpacity(0.0f);
}


