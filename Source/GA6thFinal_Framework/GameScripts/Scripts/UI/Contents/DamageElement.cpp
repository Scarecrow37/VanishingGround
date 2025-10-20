#include "pchScripts.h"
#include "DamageElement.h"

#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(DamageElement)

DamageElement::DamageElement()
    : _origin(POINT{}), _distance(0), _angle(0), _duration(0), _elapsedTime(0), _beginFontSize(0), _endFontSize(0),
      _beginRevelationFontSize(0), _endRevelationFontSize(0)
{
    _steps.reserve(3);
    _steps.push_back(0.0f);
    _steps.push_back(0.5f);
    _steps.push_back(1.0f);
    _points.reserve(3);
};

void DamageElement::Setup(const LONG distance, const float angle, const float duration, const POINT origin, const File::Guid& fontGuid, const float beginFontSize, const float endFontSize, const Color
                          fontColor, const std::string& damage, const std::span<std::string> revelations)
{
    _distance = distance;
    _angle = angle;
    _duration = duration;
    _elapsedTime = 0.0f;
    _origin      = origin;
    _beginFontSize = beginFontSize;
    _endFontSize   = endFontSize;
    _beginRevelationFontSize = beginFontSize / REVELATION_FONT_SIZE_RATIO;
    _endRevelationFontSize   = endFontSize / REVELATION_FONT_SIZE_RATIO;
    SetupPoints();
    SetupChildren(fontGuid, fontColor, damage, revelations);
    InvalidateMeasure();
}

SIZE DamageElement::MeasureOverride(const SIZE availableSize)
{
    SIZE desiredSize = {};

    SIZE damageSize = {};

    if (const auto damageTextElement = _damageTextElement.lock(); nullptr != damageTextElement)
    {
        damageTextElement->Measure(availableSize);
        damageSize  = damageTextElement->DesiredSize;
    }

    SIZE revelationSize = {};

    std::ranges::for_each(_revelationTextElements, [availableSize, &revelationSize](const std::weak_ptr<TextElement>& revelationTextElementWeakPtr) {
        if (const auto revelationTextElement = revelationTextElementWeakPtr.lock(); nullptr != revelationTextElement)
        {
            revelationTextElement->Measure(availableSize);
            const SIZE revelationDesiredSize  = revelationTextElement->DesiredSize;
            revelationSize.cx           = std::max(revelationSize.cx, revelationDesiredSize.cx);
            revelationSize.cy += revelationDesiredSize.cy;
        }
    });

    desiredSize.cx = damageSize.cx + revelationSize.cx + GAP;
    desiredSize.cy = std::max(damageSize.cy, revelationSize.cy);

    return desiredSize;
}

SIZE DamageElement::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE actualSize  = MinSize()(finalSize, desiredSize);

    const LONG revelationYStep = actualSize.cy / 3L;

    const POINT revelationPoint = AbsoluteChildPosition;

    LONG xOffset = 0L;

    for (size_t i = 0; i < _revelationTextElements.size(); ++i)
    {
        if (const auto revelationTextElement = _revelationTextElements[i].lock(); nullptr != revelationTextElement)
        {
            const POINT currentRevelationPoint = {revelationPoint.x,
                                                  revelationPoint.y + (2L - static_cast<LONG>(i)) * revelationYStep};
            revelationTextElement->Arrange(currentRevelationPoint, actualSize);
            const SIZE revelationSize = revelationTextElement->ActualSize;
            xOffset                   = std::max(xOffset, revelationSize.cx);
        }
    }

    POINT damagePoint = revelationPoint;
    damagePoint.x += xOffset + GAP;

    if (const auto damageTextElement = _damageTextElement.lock(); nullptr != damageTextElement)
    {
        damageTextElement->Arrange(damagePoint, actualSize);
    }

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

    const float t = GetT();
    UpdatePoint(t);
    UpdateFontSize(t);
}

POINT DamageElement::GetPoint(const float t) const
{
    const Vector3 point  = Mathf::CatmullRomSpline(_steps, _points, t);
    const POINT   result = {static_cast<LONG>(point.x), static_cast<LONG>(point.y)};

    return result;
}

void DamageElement::OnDestroy()
{

    if (auto damageTextElement = _damageTextElement.lock(); nullptr != damageTextElement)
    {
        GameObject::Destroy(damageTextElement->gameObject);
        damageTextElement.reset();
    }

    SIZE revelationSize = {};

    std::ranges::for_each(_revelationTextElements, [](
                                                       const std::weak_ptr<TextElement>& revelationTextElementWeakPtr) {
        if (auto revelationTextElement = revelationTextElementWeakPtr.lock(); nullptr != revelationTextElement)
        {
            GameObject::Destroy(revelationTextElement->gameObject);
            revelationTextElement.reset();
        }
    });

    UIComponent::OnDestroy();
}

float DamageElement::GetT() const
{
    return _elapsedTime / _duration;
}

void DamageElement::UpdatePoint(const float t)
{
    Point = GetPoint(t);
}

void DamageElement::UpdateFontSize(const float t) const
{
    if (const auto damageText = _damageTextElement.lock(); nullptr != damageText)
    {
        damageText->FontScale = Mathf::Lerp(_beginFontSize, _endFontSize, t);
    }

    std::ranges::for_each(
        _revelationTextElements, [t, this](const std::weak_ptr<TextElement>& revelationTextElementWeakPtr) {
            if (const auto revelationText = revelationTextElementWeakPtr.lock(); nullptr != revelationText)
            {
                revelationText->FontScale = Mathf::Lerp(_beginRevelationFontSize, _endRevelationFontSize, t);
            }
        });
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

void DamageElement::SetupChildren(const File::Guid& fontGuid, const Color fontColor, const std::string& damage, const std::span<std::string>
                                  revelations)
{
    _damageTextElement = CreateTextElement(fontGuid, damage, _beginFontSize, fontColor);
    for (auto& revelation : revelations)
    {
        _revelationTextElements.reserve(revelations.size());
        _revelationTextElements.push_back(CreateTextElement(fontGuid, revelation, _beginRevelationFontSize, fontColor));
    }
  }

std::weak_ptr<TextElement> DamageElement::CreateTextElement(const File::Guid& fontGuid, const std::string& content, const float fontSize, const Color fontColor) const
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


    return std::static_pointer_cast<TextElement, Component>(element.GetWeakPtr().lock());
}