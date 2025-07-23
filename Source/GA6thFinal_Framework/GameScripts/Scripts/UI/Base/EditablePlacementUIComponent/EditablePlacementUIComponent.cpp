#include "pchScripts.h"
#include "EditablePlacementUIComponent.h"

EditablePlacementUIComponent::EditablePlacementUIComponent() = default;

void EditablePlacementUIComponent::DrawDebug()
{
    UIComponent::DrawDebug();
    auto       [x, y]           = ReflectFields->Basefields.get().Point;
    const auto [scopeX, scopeY] = ReflectFields->Basefields.get().ScopePoint;
    x += scopeX;
    y += scopeY;
    const auto [cx, cy] = ReflectFields->Basefields.get().Size;

    const XMFLOAT2 leftTop = {static_cast<float>(x), static_cast<float>(y)};
    const XMFLOAT2 rightTop = {static_cast<float>(x + cx), static_cast<float>(y)};
    const XMFLOAT2 rightBottom = {static_cast<float>(x + cx), static_cast<float>(y + cy)};
    const XMFLOAT2 leftBottom  = {static_cast<float>(x), static_cast<float>(y + cy)};

    UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&leftTop), XMLoadFloat2(&rightTop), XMLoadFloat2(&rightBottom),
                           XMLoadFloat2(&leftBottom), DirectX::Colors::White);
}

void EditablePlacementUIComponent::DrawDebugSelected()
{
    UIComponent::DrawDebugSelected();
    auto       [x, y]   = ReflectFields->Basefields.get().Point;
    const auto [cx, cy] = ReflectFields->Basefields.get().Size;

    const auto [scopeX, scopeY] = ReflectFields->Basefields.get().ScopePoint;
    x += scopeX;
    y += scopeY;
    const XMFLOAT2 leftTop = {static_cast<float>(x), static_cast<float>(y)};
    const XMFLOAT2 rightTop = {static_cast<float>(x + cx), static_cast<float>(y)};
    const XMFLOAT2 rightBottom = {static_cast<float>(x + cx), static_cast<float>(y + cy)};
    const XMFLOAT2 leftBottom  = {static_cast<float>(x), static_cast<float>(y + cy)};
    UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&leftTop), XMLoadFloat2(&rightTop), XMLoadFloat2(&rightBottom),
                           XMLoadFloat2(&leftBottom), DirectX::Colors::Yellow);


    const POINT innerPoint{.x = x + 1, .y = y + 1};
    const SIZE  innerSize{.cx = cx - 2, .cy = cy - 2};
    const XMFLOAT2 innerLeftTop = {static_cast<float>(innerPoint.x), static_cast<float>(innerPoint.y)};
    const XMFLOAT2 innerRightTop = {static_cast<float>(innerPoint.x + innerSize.cx), static_cast<float>(innerPoint.y)};
    const XMFLOAT2 innerRightBottom = {static_cast<float>(innerPoint.x + innerSize.cx),
                                 static_cast<float>(innerPoint.y + innerSize.cy)};
    const XMFLOAT2 innerLeftBottom  = {static_cast<float>(innerPoint.x), static_cast<float>(innerPoint.y + innerSize.cy)};
    UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&innerLeftTop), XMLoadFloat2(&innerRightTop),
                           XMLoadFloat2(&innerRightBottom), XMLoadFloat2(&innerLeftBottom), DirectX::Colors::Yellow);

    const POINT outerPoint{.x = x - 1, .y = y - 1};
    const SIZE  outerSize{.cx = cx + 2, .cy = cy + 2};
    const XMFLOAT2 outerLeftTop = {static_cast<float>(outerPoint.x), static_cast<float>(outerPoint.y)};
    const XMFLOAT2 outerRightTop = {static_cast<float>(outerPoint.x + outerSize.cx), static_cast<float>(outerPoint.y)};
    const XMFLOAT2 outerRightBottom = {static_cast<float>(outerPoint.x + outerSize.cx),
                                 static_cast<float>(outerPoint.y + outerSize.cy)};
    const XMFLOAT2 outerLeftBottom  = {static_cast<float>(outerPoint.x), static_cast<float>(outerPoint.y + outerSize.cy)};
    UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&outerLeftTop), XMLoadFloat2(&outerRightTop),
                           XMLoadFloat2(&outerRightBottom), XMLoadFloat2(&outerLeftBottom), DirectX::Colors::Yellow);
}

void EditablePlacementUIComponent::ImGuiDrawPropertysEvent()
{
    PlacementUIComponent::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Reset Placement"))
    {
        ResetPlacement();
    }
}