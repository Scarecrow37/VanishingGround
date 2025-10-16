#include "pch.h"
#include "UIComponent.h"

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

SIZE MaxSize::operator()(const SIZE& lhs, const SIZE& rhs) const
{
    return SIZE{.cx = std::max(lhs.cx, rhs.cx), .cy = std::max(lhs.cy, rhs.cy)};
}

POINT AlignPoint::operator()(const HorizontalAlignment horizontal, const VerticalAlignment vertical,
                             const SIZE size) const
{
    return POINT{.x = operator()(horizontal, size.cx), .y = operator()(vertical, size.cy)};
}

LONG AlignPoint::operator()(const HorizontalAlignment horizontal, const LONG spareWidth) const
{
    LONG horizontalAlign = 0;
    switch (horizontal)
    {
    case HorizontalAlignment::LEFT:
        break;
    case HorizontalAlignment::CENTER:
        horizontalAlign += spareWidth / 2;
        break;
    case HorizontalAlignment::RIGHT:
        horizontalAlign += spareWidth;
        break;
    }
    return horizontalAlign;
}

LONG AlignPoint::operator()(const VerticalAlignment vertical, const LONG spareHeight) const
{
    LONG verticalAlign = 0;
    switch (vertical)
    {
    case VerticalAlignment::TOP:
        break;
    case VerticalAlignment::CENTER:
        verticalAlign += spareHeight / 2;
        break;
    case VerticalAlignment::BOTTOM:
        verticalAlign += spareHeight;
        break;
    }
    return verticalAlign;
}

UIRoot* UIComponent::GetRoot(const GameObject& obj)
{
    return obj.GetComponent<UIRoot>();
}

UIComponent::UIComponent()
    : _requestedPoint{}, _requestedSize{}, _isFocus(false), _isMeasureDirty(false), _isArrangeDirty(false)
{
}

void UIComponent::OnAttachChild(GameObject* childGameObject)
{
    UIBaseComponent::OnAttachChild(childGameObject);

    InvalidateMeasure();
    InvalidateArrange();
}

void UIComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIBaseComponent::OnDetachParent(previousParentGameObject);

    InvalidateMeasure();
    InvalidateArrange();
}

void UIComponent::OnDrawDebugOverride()
{
    UIBaseComponent::OnDrawDebugOverride();

    const POINT absolutePoint = AbsolutePosition;
    const SIZE  size          = Size;
    const bool  isFocus       = IsFocus;

    DrawDebug()(absolutePoint, size, 1, isFocus ? Colors::Purple : Colors::White);
}

void UIComponent::OnDrawDebugSelectedOverride()
{
    UIBaseComponent::OnDrawDebugSelectedOverride();

    const POINT absolutePoint = AbsolutePosition;
    const SIZE  size          = Size;
    const bool  isFocus       = IsFocus;

    DrawDebug()(absolutePoint, size, 3, isFocus ? Colors::Purple : Colors::Yellow);
}

void UIComponent::RequestViewOrder() const
{
    if (const UIRoot* uiRoot = this->Root; nullptr != uiRoot)
    {
        uiRoot->SortViewOrder();
    }
    else if (false == UmCore->IsPlay())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Component는 UIRoot의 하위에 있어야 합니다.");
    }
}

void UIComponent::RequestCheckNavigationIdFlawless() const
{
    if (UIRoot* uiRoot = this->Root; nullptr != uiRoot)
    {
        uiRoot->CheckNavigationIdFlawless(this);
    }
    else if (false == UmCore->IsPlay())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Component는 UIRoot의 하위에 있어야 합니다.");
    }
}

void UIComponent::ImGuiDrawPropertysEvent()
{
    UIBaseComponent::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Reset Placement"))
    {
        ResetPlacement();
    }

    if (_isDebug)
    {
        constexpr ImGuiDebug imGuiDebug;

        auto [actualX, actualY] = ReflectFields->ActualPosition;
        imGuiDebug("Actual Position", actualX, actualY);

        auto [actualWidth, actualHeight] = ReflectFields->ActualSize;
        imGuiDebug("Actual Size", actualWidth, actualHeight);

        auto [offsetX, offsetY] = ReflectFields->Offset;
        imGuiDebug("Offset", offsetX, offsetY);

        auto [desiredWidth, desiredHeight] = ReflectFields->DesiredSize;
        imGuiDebug("Desired Size", desiredWidth, desiredHeight);

        auto [availableWidth, availableHeight] = ReflectFields->AvailableSize;
        imGuiDebug("Available Size", availableWidth, availableHeight);
    }
}

void UIComponent::DeserializedReflectEvent()
{
    Component::DeserializedReflectEvent();

    _requestedPoint = ActualPosition;
    _requestedSize  = ActualSize;
}

void UIComponent::Reset()
{
    UIBaseComponent::Reset();

    InvalidateMeasure();
    InvalidateArrange();
}

void UIComponent::Start()
{
    UIBaseComponent::Start();

    InvalidateMeasure();
}

std::weak_ptr<UIComponent> UIComponent::GetUIWeakPtr() const
{
    const std::shared_ptr<UIComponent> uiComponent =
        std::static_pointer_cast<UIComponent>(GetWeakPtr().lock());
    return uiComponent;
}

void UIComponent::InvalidateMeasure()
{
    if (const bool enableInHierarchy = EnableInHierarchy; true == enableInHierarchy && false == _isMeasureDirty)
    {
        UmUI.AddMeasureQueue(GetUIWeakPtr());

        if (UIComponent* parent = Parent; nullptr != parent)
        {
            parent->InvalidateMeasure();
        }

        _isMeasureDirty = true;
    }
}

void UIComponent::InvalidateArrange()
{
    if (const bool enableInHierarchy = EnableInHierarchy; true == enableInHierarchy && false == _isArrangeDirty)
    {
        UmUI.AddArrangeQueue(GetUIWeakPtr());

        _isArrangeDirty = true;
    }
}

void UIComponent::OnAttachParent(GameObject* parentGameObject)
{
    UIBaseComponent::OnAttachParent(parentGameObject);

    RequestViewOrder();
    RequestCheckNavigationIdFlawless();
    InvalidateMeasure();
    InvalidateArrange();
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

void ImGuiDebug::operator()(const char* label) const
{
    ImGui::SameLine();
    ImGui::Text(label);
}

void ImGuiDebug::operator()(const char* label, const long x) const
{
    ImGui::Text("%d", x);
    operator()(label);
}

void ImGuiDebug::operator()(const char* label, const long x, const long y) const
{
    ImGui::Text("%d x %d", x, y);
    operator()(label);
}

void ImGuiDebug::operator()(const char* label, const unsigned int x) const
{
    ImGui::Text("%d", x);
    operator()(label);
}

void ImGuiDebug::operator()(const char* label, const int x) const
{
    ImGui::Text("%d", x);
    operator()(label);
}

void ImGuiDebug::operator()(const char* label, const size_t x) const
{
    ImGui::Text("%d", x);
    operator()(label);
}

void ImGuiDebug::operator()(const char* label, const std::string& str) const
{
    ImGui::Text(str.c_str());
    operator()(label);
}

void DrawDebug::operator()(POINT point, SIZE size, const int thickness, FXMVECTOR color) const {
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

void DrawDebug::operator()(const POINT pointA, const POINT pointB, const int thickness, const bool isVertical, FXMVECTOR color) const
{
    const int begin = 0 - thickness / 2;
    const int end   = thickness / 2;

    for (int i = begin; i <= end; ++i)
    {
        const XMFLOAT2 pointAOffset{isVertical ? static_cast<float>(pointA.x + i) : static_cast<float>(pointA.x),
                                    isVertical ? static_cast<float>(pointA.y) : static_cast<float>(pointA.y + i)};
        const XMFLOAT2 pointBOffset{isVertical ? static_cast<float>(pointB.x + i) : static_cast<float>(pointB.x),
                                    isVertical ? static_cast<float>(pointB.y) : static_cast<float>(pointB.y + i)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&pointAOffset), XMLoadFloat2(&pointBOffset), color);
    }
}

void DrawDebug::operator()(POINT pointA, POINT pointB, float arrowheadLength, FXMVECTOR color) const
{
    const XMFLOAT2 start{static_cast<float>(pointA.x), static_cast<float>(pointA.y)};
    const XMFLOAT2 end{static_cast<float>(pointB.x), static_cast<float>(pointB.y)};
    const XMVECTOR startVector = XMLoadFloat2(&start);
    const XMVECTOR endVector   = XMLoadFloat2(&end);

    UmGraphics.DebugDraw2D("Editor", startVector, endVector, color);

    // Arrowhead
    XMVECTOR direction = XMVectorSubtract(startVector, endVector);
    direction          = XMVector3Normalize(direction);

    XMVECTOR headInOrigin = XMVectorScale(direction, arrowheadLength);
    float    angle        = XMConvertToRadians(20.0f);

    // right head
    XMMATRIX rightRotationMatrix = XMMatrixRotationZ(angle);
    XMVECTOR rightHead           = XMVector2Transform(headInOrigin, rightRotationMatrix);
    rightHead                    = XMVectorAdd(rightHead, endVector);
    UmGraphics.DebugDraw2D("Editor", rightHead, endVector, color);

    // left head
    XMMATRIX leftRotationMatrix = XMMatrixRotationZ(-angle);
    XMVECTOR leftHead           = XMVector2Transform(headInOrigin, leftRotationMatrix);
    leftHead                    = XMVectorAdd(leftHead, endVector);
    UmGraphics.DebugDraw2D("Editor", leftHead, endVector, color);
}

namespace
{
    bool IntersectSegments(const XMVECTOR& p1, const XMVECTOR& p2, const XMVECTOR& q1, const XMVECTOR& q2,
                           XMVECTOR& out)
    {
        const float x1 = XMVectorGetX(p1), y1 = XMVectorGetY(p1);
        const float x2 = XMVectorGetX(p2), y2 = XMVectorGetY(p2);
        const float x3 = XMVectorGetX(q1), y3 = XMVectorGetY(q1);
        const float x4 = XMVectorGetX(q2), y4 = XMVectorGetY(q2);

        const float dx1 = x2 - x1, dy1 = y2 - y1;
        const float dx2 = x4 - x3, dy2 = y4 - y3;
        const float det = dx1 * dy2 - dy1 * dx2;
        if (fabs(det) < 1e-6f)
            return false; // 평행

        const float s = ((dx2) * (y1 - y3) - (dy2) * (x1 - x3)) / det;
        const float t = ((dx1) * (y1 - y3) - (dy1) * (x1 - x3)) / det;

        if (s < 0 || s > 1 || t < 0 || t > 1)
            return false; // 교차 영역 밖
        out = XMVectorSet(x1 + s * dx1, y1 + s * dy1, 0.0f, 0.0f);
        return true;
    }
} // namespace

std::optional<std::pair<POINT, POINT>> DrawDebug::operator()(const POINT pointA, const POINT pointB, const RECT rectA, const RECT rectB) const
{
    std::optional<std::pair<POINT, POINT>> result = std::nullopt;

    XMVECTOR vectorA = XMVectorSet(static_cast<float>(pointA.x), static_cast<float>(pointA.y), 0.0f, 0.0f);
    XMVECTOR vectorB = XMVectorSet(static_cast<float>(pointB.x), static_cast<float>(pointB.y), 0.0f, 0.0f);

    XMVECTOR edgesA[4][2] = {
        {XMVectorSet(static_cast<float>(rectA.left), static_cast<float>(rectA.top), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectA.right), static_cast<float>(rectA.top), 0.0f, 0.0f)},
        {XMVectorSet(static_cast<float>(rectA.right), static_cast<float>(rectA.top), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectA.right), static_cast<float>(rectA.bottom), 0.0f, 0.0f)},
        {XMVectorSet(static_cast<float>(rectA.right), static_cast<float>(rectA.bottom), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectA.left), static_cast<float>(rectA.bottom), 0.0f, 0.0f)},
        {XMVectorSet(static_cast<float>(rectA.left), static_cast<float>(rectA.bottom), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectA.left), static_cast<float>(rectA.top), 0.0f, 0.0f)},
    };

    XMVECTOR edgesB[4][2] = {
        {XMVectorSet(static_cast<float>(rectB.left), static_cast<float>(rectB.top), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectB.right), static_cast<float>(rectB.top), 0.0f, 0.0f)},
        {XMVectorSet(static_cast<float>(rectB.right), static_cast<float>(rectB.top), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectB.right), static_cast<float>(rectB.bottom), 0.0f, 0.0f)},
        {XMVectorSet(static_cast<float>(rectB.right), static_cast<float>(rectB.bottom), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectB.left), static_cast<float>(rectB.bottom), 0.0f, 0.0f)},
        {XMVectorSet(static_cast<float>(rectB.left), static_cast<float>(rectB.bottom), 0.0f, 0.0f),
         XMVectorSet(static_cast<float>(rectB.left), static_cast<float>(rectB.top), 0.0f, 0.0f)},
    };

    XMVECTOR intersectionA, intersectionB;
    bool     foundA = false, foundB = false;

    for (const auto& edge : edgesA)
    {
        if (IntersectSegments(vectorA, vectorB, edge[0], edge[1], intersectionA))
        {
            foundA = true;
            break;
        }
    }

    for (const auto& edge : edgesB)
    {
        if (IntersectSegments(vectorA, vectorB, edge[0], edge[1], intersectionB))
        {
            foundB = true;
            break;
        }
    }

    if (foundA && foundB)
    {
        POINT intersectPointA = {.x = static_cast<LONG>(XMVectorGetX(intersectionA)),
                                 .y = static_cast<LONG>(XMVectorGetY(intersectionA))};
        POINT intersectPointB = {.x = static_cast<LONG>(XMVectorGetX(intersectionB)),
                                 .y = static_cast<LONG>(XMVectorGetY(intersectionB))};
        result                = std::make_pair(intersectPointA, intersectPointB);
    }


    return result;
}

void DrawDebug::operator()(const POINT origin, const float radius, FXMVECTOR color) const
{
    const XMVECTOR originVector = XMVectorSet(static_cast<float>(origin.x), static_cast<float>(origin.y), 0.0f, 0.0f);
    UmGraphics.DebugDraw2D("Editor", originVector, radius, color);
}
