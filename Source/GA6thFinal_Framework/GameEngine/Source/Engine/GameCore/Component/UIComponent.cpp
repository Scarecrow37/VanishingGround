#include "pch.h"
#include "UIComponent.h"

bool UIComponent::_isDebug = false;

bool operator==(const POINT& lhs, const POINT& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const POINT& lhs, const POINT& rhs)
{
    return !(lhs == rhs);
}

POINT operator+(const POINT& lhs, const POINT& rhs)
{
    return POINT{lhs.x + rhs.x, lhs.y + rhs.y};
}

POINT operator-(const POINT& lhs, const POINT& rhs)
{
    return POINT{lhs.x - rhs.x, lhs.y - rhs.y};
}

POINT& operator+=(POINT& lhs, const POINT& rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

bool operator==(const SIZE& lhs, const SIZE& rhs)
{
    return lhs.cx == rhs.cx && lhs.cy == rhs.cy;
}

bool operator!=(const SIZE& lhs, const SIZE& rhs)
{
    return !(lhs == rhs);
}

SIZE operator+(const SIZE& lhs, const SIZE& rhs)
{
    return SIZE{lhs.cx + rhs.cx, lhs.cy + rhs.cy};
}

SIZE operator-(const SIZE& lhs, const SIZE& rhs)
{
    return SIZE{lhs.cx - rhs.cx, lhs.cy - rhs.cy};
}

SIZE& operator+=(SIZE& lhs, const SIZE& rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

SIZE MinSize::operator()(const SIZE& lhs, const SIZE& rhs) const
{
    return SIZE{.cx = std::min(lhs.cx, rhs.cx), .cy = std::min(lhs.cy, rhs.cy)};
}

SIZE MinSize::operator()(const SIZE& lhs, const SIZE& rhs, const bool useLhsWidth, const bool useLhsHeight) const
{
    return SIZE{.cx = useLhsWidth ? lhs.cx : std::min(lhs.cx, rhs.cx),
                .cy = useLhsHeight ? lhs.cy : std::min(lhs.cy, rhs.cy)};
}

POINT AlignPoint::operator()(const HorizontalAlignment horizontal, const VerticalAlignment vertical,
                             const SIZE size) const
{
    POINT alignPoint{};
    switch (horizontal)
    {
    case HorizontalAlignment::LEFT:
        break;
    case HorizontalAlignment::CENTER:
        alignPoint.x += size.cx / 2;
        break;
    case HorizontalAlignment::RIGHT:
        alignPoint.x += size.cx;
        break;
    }

    switch (vertical)
    {
    case VerticalAlignment::TOP:
        break;
    case VerticalAlignment::CENTER:
        alignPoint.y += size.cy / 2;
        break;
    case VerticalAlignment::BOTTOM:
        alignPoint.y += size.cy;
        break;
    }

    return alignPoint;
}

UIComponent::UIComponent()
    : Component(TYPE::UI), _requestedPoint{}, _requestedSize{}, _isMeasureDirty(false),
      _isArrangeDirty(false)
{
}

void UIComponent::OnAttachChild(GameObject* childGameObject)
{
    InvalidateMeasure();
}

void UIComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    InvalidateMeasure();
}

void UIComponent::OnDrawDebugOverride()
{
    const POINT absolutePoint = AbsolutePosition;
    const SIZE  size          = Size;

    DrawQuad()(absolutePoint, size, 1, Colors::White);
}

void UIComponent::OnDrawDebugSelectedOverride()
{
    const POINT absolutePoint = AbsolutePosition;
    const SIZE  size          = Size;

    DrawQuad()(absolutePoint, size, 3, Colors::Yellow);
}

void UIComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Reset Placement"))
    {
        ResetPlacement();
    }

    if (ImGui::Button("Debug"))
    {
        _isDebug = !_isDebug;
    }

    if (_isDebug)
    {
        constexpr DrawDebug drawDebug;

        auto [actualX, actualY] = ReflectFields->ActualPosition;
        drawDebug("Actual Position", actualX, actualY);

        auto [actualWidth, actualHeight] = ReflectFields->ActualSize;
        drawDebug("Actual Size", actualWidth, actualHeight);

        auto [offsetX, offsetY] = ReflectFields->Offset;
        drawDebug("Offset", offsetX, offsetY);

        auto [desiredWidth, desiredHeight] = ReflectFields->DesiredSize;
        drawDebug("Desired Size", desiredWidth, desiredHeight);

        auto [availableWidth, availableHeight] = ReflectFields->AvailableSize;
        drawDebug("Available Size", availableWidth, availableHeight);
    }
}

void UIComponent::DeserializedReflectEvent()
{
    Component::DeserializedReflectEvent();

    _requestedPoint = ActualPosition;
    _requestedSize  = ActualSize;
}

void UIComponent::InvalidateMeasure()
{
    if (false == _isMeasureDirty)
    {
        UmUI.AddMeasureQueue(this);

        if (UIComponent* parent = Parent; nullptr != parent)
        {
            parent->InvalidateMeasure();
        }

        _isMeasureDirty = true;
    }
}

void UIComponent::InvalidateArrange()
{
    if (false == _isArrangeDirty)
    {
        UmUI.AddArrangeQueue(this);

        _isArrangeDirty = true;
    }
}

void UIComponent::Measure(const SIZE availableSize)
{
    ReflectFields->AvailableSize = availableSize;

    const SIZE desiredSize = ReflectFields->DesiredSize;

    const MARGIN margin                = Margin;
    const SIZE   overrideAvailableSize = availableSize - margin.Size();
    if (const SIZE newDesiredSize = MeasureOverride(overrideAvailableSize); desiredSize != newDesiredSize)
    {
        ReflectFields->DesiredSize = newDesiredSize;
        InvalidateArrange();
    }

    _isMeasureDirty = false;
}

void UIComponent::Arrange(const POINT finalPosition, const SIZE finalSize)
{
    ReflectFields->Offset         = finalPosition;
    ReflectFields->ActualPosition = _requestedPoint;
    ReflectFields->ActualSize     = ArrangeOverride(finalSize);

    _isArrangeDirty = false;
}

void UIComponent::Arrange()
{
    const SIZE finalSize      = ReflectFields->ActualSize;
    ReflectFields->ActualSize = ArrangeOverride(finalSize);
}

void UIComponent::ResetPlacement()
{
    _requestedSize  = ReflectFields->AvailableSize;
    InvalidateMeasure();
    _requestedPoint = {.x = 0, .y = 0};
    InvalidateArrange();
}

void UIComponent::OnDrawDebug()
{
    Component::OnDrawDebug();

    if (const bool isEnable = EnableInHierarchy; isEnable)
    {
        OnDrawDebugOverride();
    }
}

void UIComponent::OnDrawDebugSelected()
{
    Component::OnDrawDebugSelected();

    if (const bool isEnable = EnableInHierarchy; isEnable)
    {
        OnDrawDebugSelectedOverride();
    }
}

void DrawDebug::operator()(const char* label) const
{
    ImGui::SameLine();
    ImGui::Text(label);
}

void DrawDebug::operator()(const char* label, const long x) const
{
    ImGui::Text("%d", x);
    operator()(label);
}

void DrawDebug::operator()(const char* label, const long x, const long y) const
{
    ImGui::Text("%d x %d", x, y);
    operator()(label);
}

void DrawQuad::operator()(POINT point, SIZE size, const int thickness, FXMVECTOR color) const {
    const auto [x, y]          = point;
    const auto [width, height] = size;

    const POINT leftTop{.x = x, .y = y};
    const POINT rightTop{.x = x + width, .y = y};
    const POINT rightBottom{.x = x + width, .y = y + height};
    const POINT leftBottom{.x = x, .y = y + height};

    const int begin = 0 - thickness / 2;
    const int end   = thickness / 2;

    for (int i = begin; i <= end; ++i)
    {
        const XMFLOAT2 leftTopVector{static_cast<float>(leftTop.x + i), static_cast<float>(leftTop.y + i)};
        const XMFLOAT2 rightTopVector{static_cast<float>(rightTop.x - i), static_cast<float>(rightTop.y + i)};
        const XMFLOAT2 rightBottomVector{static_cast<float>(rightBottom.x - i),
                                         static_cast<float>(rightBottom.y - i)};
        const XMFLOAT2 leftBottomVector{static_cast<float>(leftBottom.x + i), static_cast<float>(leftBottom.y - i)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&leftTopVector), XMLoadFloat2(&rightTopVector),
                               XMLoadFloat2(&rightBottomVector), XMLoadFloat2(&leftBottomVector), color);
    }
}