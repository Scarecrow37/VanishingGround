#include "pchScripts.h"
#include "DamageElement.h"

#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(DamageElement)

DamageElement::DamageElement() : _origin(POINT{}), _distance(0), _angle(0), _duration(0), _elapsedTime(0),
                                 _damageTextElement(nullptr)
{
    _steps.reserve(3);
    _steps.push_back(0.0f);
    _steps.push_back(0.5f);
    _steps.push_back(1.0f);
    _points.reserve(3);
};

void DamageElement::Setup(const LONG distance, const float angle, const float duration, const POINT origin, const File::Guid& fontGuid, const float fontSize, const Color
                          fontColor, const std::string& damage, const std::span<std::string> revelations)
{
    _distance = distance;
    _angle = angle;
    _duration = duration;
    _elapsedTime = 0.0f;
    _origin      = origin;
    SetupPoints();
    SetupChildren(fontGuid, fontSize, fontColor, damage, revelations);
    InvalidateMeasure();
}

SIZE DamageElement::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    const SIZE     desiredSize        = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                       verticalFillMode == FillMode::FILL);

    return desiredSize;
}

SIZE DamageElement::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE actualSize  = MinSize()(finalSize, desiredSize);
    return actualSize;
}

void DamageElement::Update()
{
    UIComponent::Update();

    _elapsedTime += UmTime.DeltaTime();

    if (_elapsedTime >= _duration)
    {
        GameObject::Destroy(gameObject);
        return;
    }

    Point = GetPoint();
}

POINT DamageElement::GetPoint() const
{
    const float   t      = _elapsedTime / _duration;
    const Vector3 point  = Mathf::CatmullRomSpline(_steps, _points, t);
    const POINT   result = {static_cast<LONG>(point.x), static_cast<LONG>(point.y)};

    return result;
}

void DamageElement::OnDestroy()
{
    if (nullptr != _damageTextElement)
    {
        GameObject::Destroy(_damageTextElement->gameObject);
        _damageTextElement = nullptr;
    }

    for (const TextElement* revelationTextElement : _revelationTextElements)
    {
        if (nullptr != revelationTextElement)
        {
            GameObject::Destroy(revelationTextElement->gameObject);
        }
    }

    UIComponent::OnDestroy();
}

void DamageElement::SetupPoints()
{
    const Matrix rotationMatrix = Matrix::CreateRotationZ(_angle);
    const Matrix originMatrix =
        Matrix::CreateTranslation(static_cast<float>(_origin.x), static_cast<float>(_origin.y), 0.0f);
    const Matrix transformMatrix = rotationMatrix * originMatrix;

    Vector3 p0 = Vector3::Zero;
    p0         = Vector3::Transform(p0, transformMatrix);
    const float   half = static_cast<float>(_distance) * 0.5f;
    Vector3 p1   = Vector3(half, -half * 0.5f, 0.0f);
    p1           = Vector3::Transform(p1, transformMatrix);
    Vector3 p2 = Vector3(static_cast<float>(_distance), 0.0f, 0.0f);
    p2         = Vector3::Transform(p2, transformMatrix);

    _points.clear();
    _points.push_back(p0);
    _points.push_back(p1);
    _points.push_back(p2);
}

void DamageElement::SetupChildren(const File::Guid& fontGuid, const float fontSize, const Color fontColor, const std::string& damage, const std::span<std::string>
                                  revelations)
{
    _damageTextElement = CreateTextElement(fontGuid, damage, fontSize, fontColor);
    const float revelationSize = fontSize / 3.0f;
    for (auto& revelation : revelations)
    {
        _revelationTextElements.reserve(revelations.size());
        _revelationTextElements.push_back(CreateTextElement(fontGuid, revelation, revelationSize, fontColor));
    }
  }

TextElement* DamageElement::CreateTextElement(const File::Guid& fontGuid, const std::string& content, const float fontSize, const Color fontColor) const
{
    Transform&                        transform = this->transform;
    const std::shared_ptr<GameObject> child     = NewGameObject(GameObject::Helper::GenerateUniqueName("Text Element"));
    TextElement&                      element   = child->AddComponent<TextElement>();
    element.SetFont(fontGuid);
    element.HorizontalFillMode = FillMode::WRAP;
    element.VerticalFillMode   = FillMode::WRAP;
    element.Text               = content;
    element.FontScale          = fontSize;
    element.Color              = fontColor;
    child->transform->SetParent(&transform);

    return &element;
}