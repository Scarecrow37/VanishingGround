#include "pchScripts.h"
#include "EditablePlacementUIComponent.h"

EditablePlacementUIComponent::EditablePlacementUIComponent() = default;

void EditablePlacementUIComponent::OnDrawDebugOverride()
{
    UIComponent::OnDrawDebugOverride();

    DrawQuad([](const POINT& leftTop, const POINT& rightTop, const POINT& rightBottom, const POINT& leftBottom) {
        const XMFLOAT2 leftTopVector     = {static_cast<float>(leftTop.x), static_cast<float>(leftTop.y)};
        const XMFLOAT2 rightTopVector    = {static_cast<float>(rightTop.x), static_cast<float>(rightTop.y)};
        const XMFLOAT2 rightBottomVector = {static_cast<float>(rightBottom.x), static_cast<float>(rightBottom.y)};
        const XMFLOAT2 leftBottomVector  = {static_cast<float>(leftBottom.x), static_cast<float>(leftBottom.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&leftTopVector), XMLoadFloat2(&rightTopVector),
                               XMLoadFloat2(&rightBottomVector), XMLoadFloat2(&leftBottomVector), Colors::White);
    });
}

void EditablePlacementUIComponent::OnDrawDebugSelectedOverride()
{
    UIComponent::OnDrawDebugSelectedOverride();

    DrawQuad([](const POINT& leftTop, const POINT& rightTop, const POINT& rightBottom, const POINT& leftBottom) {
        const XMFLOAT2 leftTopVector{static_cast<float>(leftTop.x), static_cast<float>(leftTop.y)};
        const XMFLOAT2 rightTopVector{static_cast<float>(rightTop.x), static_cast<float>(rightTop.y)};
        const XMFLOAT2 rightBottomVector{static_cast<float>(rightBottom.x), static_cast<float>(rightBottom.y)};
        const XMFLOAT2 leftBottomVector{static_cast<float>(leftBottom.x), static_cast<float>(leftBottom.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&leftTopVector), XMLoadFloat2(&rightTopVector),
                               XMLoadFloat2(&rightBottomVector), XMLoadFloat2(&leftBottomVector), Colors::Yellow);

        const XMFLOAT2 innerLeftTop{static_cast<float>(leftTop.x + 1), static_cast<float>(leftTop.y + 1)};
        const XMFLOAT2 innerRightTop{static_cast<float>(rightTop.x - 1), static_cast<float>(rightTop.y + 1)};
        const XMFLOAT2 innerRightBottom{static_cast<float>(rightBottom.x - 1), static_cast<float>(rightBottom.y - 1)};
        const XMFLOAT2 innerLeftBottom{static_cast<float>(leftBottom.x + 1), static_cast<float>(leftBottom.y - 1)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&innerLeftTop), XMLoadFloat2(&innerRightTop),
                               XMLoadFloat2(&innerRightBottom), XMLoadFloat2(&innerLeftBottom), Colors::Yellow);

        const XMFLOAT2 outerLeftTop{static_cast<float>(leftTop.x - 1), static_cast<float>(leftTop.y - 1)};
        const XMFLOAT2 outerRightTop{static_cast<float>(rightTop.x + 1), static_cast<float>(rightTop.y - 1)};
        const XMFLOAT2 outerRightBottom{static_cast<float>(rightBottom.x + 1), static_cast<float>(rightBottom.y + 1)};
        const XMFLOAT2 outerLeftBottom{static_cast<float>(leftBottom.x - 1), static_cast<float>(leftBottom.y + 1)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&outerLeftTop), XMLoadFloat2(&outerRightTop),
                               XMLoadFloat2(&outerRightBottom), XMLoadFloat2(&outerLeftBottom), Colors::Yellow);
    });
}

void EditablePlacementUIComponent::ImGuiDrawPropertysEvent()
{
    PlacementUIComponent::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Reset Placement"))
    {
        ResetPlacement();
        SpreadPlacementToParent();
    }
}

void EditablePlacementUIComponent::DrawQuad(const QuadCallback& callback) const
{
    const auto [x, y]          = GetAbsolutePoint();
    const auto [width, height] = GetSize();

    const POINT leftTop{.x = x, .y = y};
    const POINT rightTop{.x = x + width, .y = y};
    const POINT rightBottom{.x = x + width, .y = y + height};
    const POINT leftBottom{.x = x, .y = y + height};

    callback(leftTop, rightTop, rightBottom, leftBottom);
}