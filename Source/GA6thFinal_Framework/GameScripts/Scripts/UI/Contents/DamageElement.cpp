#include "pchScripts.h"
#include "DamageElement.h"

#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(DamageElement)

DamageElement::DamageElement()
    : _origin(POINT{}), _distance(0), _angle(0), _duration(0), _elapsedTime(0), _beginFontSize(0), _endFontSize(0),
      _beginRevelationFontSize(0), _endRevelationFontSize(0), _sizeRatio(0)
{
    _steps.reserve(3);
    _steps.push_back(0.0f);
    _steps.push_back(0.5f);
    _steps.push_back(1.0f);
    _points.reserve(3);
};

void DamageElement::Setup(const SetupData& data)
{
    _distance    = data.Distance;
    _angle = data.Angle;
    _duration = data.Duration;
    _elapsedTime = 0.0f;
    _origin      = data.Origin;
    if (data.Revelations.size() > 1)
    {
        _beginFontSize = data.BeginFontSize * REVELATION_FONT_SIZE_RATIO_LARGE;
        _endFontSize   = data.EndFontSize * REVELATION_FONT_SIZE_RATIO_LARGE;
        _sizeRatio     = static_cast<LONG>(REVELATION_FONT_SIZE_RATIO_LARGE);
    }
    else
    {
        _beginFontSize = data.BeginFontSize * REVELATION_FONT_SIZE_RATIO_SMALL;
        _endFontSize   = data.EndFontSize * REVELATION_FONT_SIZE_RATIO_SMALL;
        _sizeRatio     = static_cast<LONG>(REVELATION_FONT_SIZE_RATIO_SMALL);
    }
    _beginRevelationFontSize = data.BeginFontSize;
    _endRevelationFontSize   = data.EndFontSize;
    _beginColor              = data.BeginColor;
    _endColor                = data.EndColor;
    _beginOutlineColor       = data.BeginOutlineColor;
    _endOutlineColor         = data.EndOutlineColor;
    _turningPoint            = std::clamp(data.TurningPoint, 0.1f, 0.9f);
    _easingFunctionType      = data.EasingFunctionType;
    SetupPoints();
    SetupChildren(data.FontGuid, data.Damage, data.Revelations);
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

    if (const auto damageOutlineElement = _damageOutlineElement.lock(); nullptr != damageOutlineElement)
    {
        damageOutlineElement->Measure(availableSize);
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

    std::ranges::for_each(_revelationOutlineElements,
                          [availableSize](const std::weak_ptr<TextElement>& revelationOutlineElementWeakPtr) {
                              if (const auto revelationOutlineElement = revelationOutlineElementWeakPtr.lock();
                                  nullptr != revelationOutlineElement)
                              {
                                  revelationOutlineElement->Measure(availableSize);
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

    const LONG revelationYStep = actualSize.cy / _sizeRatio;

    const POINT revelationPoint = AbsoluteChildPosition;

    LONG xOffset = 0L;

    for (size_t i = 0; i < _revelationTextElements.size(); ++i)
    {
        if (const auto revelationTextElement = _revelationTextElements[i].lock(); nullptr != revelationTextElement)
        {
            SIZE revelationDesiredSize = revelationTextElement->DesiredSize;
            xOffset                    = std::max(xOffset, revelationDesiredSize.cx);
        }
    }

    for (size_t i = 0; i < _revelationTextElements.size(); ++i)
    {
        POINT textPoint = {};
        SIZE  textSize = {};
        if (const auto revelationTextElement = _revelationTextElements[i].lock(); nullptr != revelationTextElement)
        {
            const SIZE  revelationDesiredSize = revelationTextElement->DesiredSize;
            const POINT alignPosition =
                AlignPoint()(HorizontalAlignment::RIGHT, VerticalAlignment::TOP,
                             SIZE{.cx = xOffset, .cy = 0} - SIZE{.cx = revelationDesiredSize.cx, .cy = 0});
            const POINT currentRevelationPoint = {
                revelationPoint.x, revelationPoint.y + ((_sizeRatio - 1) - static_cast<LONG>(i)) * revelationYStep};
            const POINT childPoint = currentRevelationPoint + alignPosition;
            revelationTextElement->Arrange(childPoint, actualSize);
            textPoint = childPoint;
            textSize  = revelationDesiredSize;
        }

        if (const auto revelationOutlineElement = _revelationOutlineElements[i].lock(); nullptr != revelationOutlineElement)
        {
            const SIZE revelationOutlineDesiredSize = revelationOutlineElement->DesiredSize;
            textPoint.x -= (revelationOutlineDesiredSize.cx - textSize.cx);
            textPoint.y -= ((revelationOutlineDesiredSize.cy - textSize.cy) / 2) * static_cast<LONG>(i);
            revelationOutlineElement->Arrange(textPoint, actualSize);
        }
    }

    POINT damagePoint = revelationPoint;
    damagePoint.x += xOffset + GAP;
    SIZE damageDesiredSize = {};

    if (const auto damageTextElement = _damageTextElement.lock(); nullptr != damageTextElement)
    {
        damageDesiredSize = damageTextElement->DesiredSize;
        damageTextElement->Arrange(damagePoint, actualSize);
    }

    if (const auto damageOutlineElement = _damageOutlineElement.lock(); nullptr != damageOutlineElement)
    {
        const SIZE damageOutlineDesiredSize = damageOutlineElement->DesiredSize;
        damagePoint.y -= (damageOutlineDesiredSize.cy - damageDesiredSize.cy) / 2;
        damageOutlineElement->Arrange(damagePoint, actualSize);
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
    UpdateFont(t);
}

POINT DamageElement::GetPoint(const float t) const
{
    const Vector3 pointF  = Mathf::CatmullRomSpline(_steps, _points, t);
    const POINT   point = {static_cast<LONG>(pointF.x), static_cast<LONG>(pointF.y)};
    const SIZE    size   = Size;
    const POINT   result  = {point.x - size.cx / 2, point.y - size.cy / 2};

    return result;
}

float DamageElement::GetT() const
{
    return _elapsedTime / _duration;
}

void DamageElement::UpdatePoint(const float t)
{
    Point = GetPoint(t);
}

void DamageElement::UpdateFont(const float t) const
{
    float beforeTurningPointT = std::clamp(t / _turningPoint, 0.0f, 1.0f);
    float easingT =
        Mathf::Ease(Mathf::EASE_IN, Mathf::ConvertEasingFunctionType()(_easingFunctionType), 1.0f, beforeTurningPointT);
    Color updateColor        = Color::Lerp(_beginColor, _endColor, easingT);
    Color updateOutlineColor = Color::Lerp(_beginOutlineColor, _endOutlineColor, easingT);
    if (const auto damageText = _damageTextElement.lock(); nullptr != damageText)
    {
        damageText->FontScale    = Mathf::Lerp(_beginFontSize, _endFontSize, easingT);
        damageText->Color        = updateColor;
        damageText->OutlineColor = updateOutlineColor;
    }

    std::ranges::for_each(_revelationTextElements, [easingT, updateColor, updateOutlineColor, this](
                                                       const std::weak_ptr<TextElement>& revelationTextElementWeakPtr) {
        if (const auto revelationText = revelationTextElementWeakPtr.lock(); nullptr != revelationText)
        {
            revelationText->FontScale    = Mathf::Lerp(_beginRevelationFontSize, _endRevelationFontSize, easingT);
            revelationText->Color        = updateColor;
            revelationText->OutlineColor = updateOutlineColor;
        }
    });

    if (t < _turningPoint)
    {
        if (const auto damageOutline = _damageOutlineElement.lock(); nullptr != damageOutline)
        {
            damageOutline->FontScale    = Mathf::Lerp(_beginFontSize, _endFontSize, beforeTurningPointT);
            damageOutline->OutlineColor = updateOutlineColor;
        }

        std::ranges::for_each(
            _revelationOutlineElements,
            [beforeTurningPointT, updateOutlineColor, this](const std::weak_ptr<TextElement>& revelationOutlineElementWeakPtr) {
                if (const auto revelationOutline = revelationOutlineElementWeakPtr.lock(); nullptr != revelationOutline)
                {
                    revelationOutline->FontScale = Mathf::Lerp(_beginRevelationFontSize, _endRevelationFontSize, beforeTurningPointT);
                    revelationOutline->OutlineColor = updateOutlineColor;
                }
            });
    }
    else
    {
        float afterTurningPointT = std::clamp((t - _turningPoint) / (1.0f - _turningPoint), 0.0f, 1.0f);
        Color outlineColor = Color::Lerp(_endOutlineColor, _beginOutlineColor, afterTurningPointT);
        if (const auto damageOutline = _damageOutlineElement.lock(); nullptr != damageOutline)
        {
            damageOutline->FontScale = Mathf::Lerp(_endFontSize, _beginFontSize, afterTurningPointT);
            damageOutline->OutlineColor = outlineColor;
        }
 
        std::ranges::for_each(
            _revelationOutlineElements,
            [afterTurningPointT, outlineColor, this](const std::weak_ptr<TextElement>& revelationOutlineElementWeakPtr) {
                if (const auto revelationOutline = revelationOutlineElementWeakPtr.lock(); nullptr != revelationOutline)
                {
                    revelationOutline->FontScale = Mathf::Lerp(_endRevelationFontSize, _beginRevelationFontSize, afterTurningPointT);
                    revelationOutline->OutlineColor = outlineColor;
                }
            });
    }
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

void DamageElement::SetupChildren(const File::Guid& fontGuid, const std::string& damage, const std::span<std::string>
                                  revelations)
{
    _damageTextElement = CreateTextElement(fontGuid, damage, _beginFontSize, _beginColor, _beginOutlineColor);
    for (auto& revelation : revelations)
    {
        _revelationTextElements.reserve(revelations.size());
        _revelationTextElements.push_back(CreateTextElement(fontGuid, revelation, _beginRevelationFontSize, _beginColor, _beginOutlineColor));
    }

    _damageOutlineElement = CreateTextElement(fontGuid, damage, _beginFontSize, COLOR_TRANSPARENT, _beginOutlineColor);
    _revelationOutlineElements.reserve(revelations.size());
    for (auto& revelation : revelations)
    {
        _revelationOutlineElements.push_back(
            CreateTextElement(fontGuid, revelation, _beginRevelationFontSize, COLOR_TRANSPARENT, _beginOutlineColor));
    }
}

std::weak_ptr<TextElement> DamageElement::CreateTextElement(const File::Guid& fontGuid, const std::string& content,
                                                              const float                fontSize,
                                                              const Color fontColor, const Color outlineColor) const
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
    element.OutlineColor       = outlineColor;
    element.OutlineWidth       = 1.0f;
    element.Outline            = true;
    child->transform->SetParent(&transform);


    return std::static_pointer_cast<TextElement, Component>(element.GetWeakPtr().lock());
}