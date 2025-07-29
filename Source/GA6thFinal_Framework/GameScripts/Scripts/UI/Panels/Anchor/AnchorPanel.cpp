#include "pchScripts.h"
#include "AnchorPanel.h"

AnchorPanel::AnchorPanel() = default;

void AnchorPanel::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);

    auto& slot = childGameObject->AddComponent<AnchorPanelSlot>();
    AssignChild(slot);
}

void AnchorPanel::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();

    std::vector<AnchorPanelSlot*> slots = FindChildComponents<AnchorPanelSlot>()(transform);
    std::ranges::for_each(slots, [this](AnchorPanelSlot* slot) { AssignChild(*slot); });
}

void AnchorPanel::AssignChild(AnchorPanelSlot& slot) const
{
    const POINT absolutePoint = GetAbsolutePoint();
    const SIZE  size          = GetSize();
    slot.SetScopePlacement(absolutePoint, size);
}

AnchorPanelSlot::AnchorPanelSlot() = default;

AnchorPanelSlot::AnchorType AnchorPanelSlot::GetAnchorType() const
{
    return ReflectFields->Anchor;
}

POINT AnchorPanelSlot::GetAnchorPoint() const
{
    return ReflectFields->AnchorPoint;
}

POINT AnchorPanelSlot::GetOffsetPoint() const
{
    return ReflectFields->OffsetPoint;
}

SIZE AnchorPanelSlot::GetSiblingSize() const
{
    return ReflectFields->SiblingSize;
}

void AnchorPanelSlot::OnPlacementChange()
{
    PanelSlotComponent::OnPlacementChange();

    UpdateAnchorPoint();
    UpdateOffsetPoint();
    const POINT anchorPoint = GetAnchorPoint();
    const POINT offsetPoint = GetOffsetPoint();
    const SIZE  size        = GetSiblingSize();
    PassScopedPlacementToSibling(anchorPoint - offsetPoint, size);
}

void AnchorPanelSlot::ImGuiDrawPropertysEvent()
{
    PanelSlotComponent::ImGuiDrawPropertysEvent();
    static const char* items[]      = {"Top Left",     "Top Center",  "Top Right",     "Center Left", "Center",
                                       "Center Right", "Bottom Left", "Bottom Center", "Bottom Right"};
    int                currentIndex = static_cast<int>(ReflectFields->Anchor);
    if (ImGui::Combo("##Anchor", &currentIndex, items, IM_ARRAYSIZE(items)))
    {
        ReflectFields->Anchor = static_cast<AnchorType>(currentIndex);
        OnPlacementChange();
    }
    ImGui::SameLine();
    ImGui::Text("Anchor");
}

void AnchorPanelSlot::DrawDebugSelected()
{
    PanelSlotComponent::DrawDebugSelected();

    auto [anchorX, anchorY] = ReflectFields->AnchorPoint;

    constexpr LONG lineSize = 20;

    // Top to bottom line
    {
        const POINT start{anchorX, anchorY - lineSize};
        const POINT end{anchorX, anchorY + lineSize};
        const XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        const XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::Yellow);
    }

    // Left to right line
    {
        const POINT start{anchorX - lineSize, anchorY};
        const POINT end{anchorX + lineSize, anchorY};
        const XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        const XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::Yellow);
    }

    // LeftTop to RightBottom line
    {
        const POINT start{anchorX - lineSize, anchorY - lineSize};
        const POINT end{anchorX + lineSize, anchorY + lineSize};
        const XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        const XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::Yellow);
    }

    // RightTop to LeftBottom line
    {
        const POINT start{anchorX + lineSize, anchorY - lineSize};
        const POINT end{anchorX - lineSize, anchorY + lineSize};
        const XMFLOAT2 startVector = {static_cast<float>(start.x), static_cast<float>(start.y)};
        const XMFLOAT2 endVector   = {static_cast<float>(end.x), static_cast<float>(end.y)};
        UmGraphics.DebugDraw2D("Editor", XMLoadFloat2(&startVector), XMLoadFloat2(&endVector), DirectX::Colors::Yellow);
    }
}

void AnchorPanelSlot::UpdateAnchorPoint()
{
    const POINT absolutePoint  = GetAbsolutePoint();
    const auto [width, height] = GetSize();
    const AnchorType anchor    = ReflectFields->Anchor;

    auto [anchorX, anchorY] = absolutePoint;
    switch (anchor)
    {
    case AnchorType::TOP_LEFT:
        // Nothing to do here, as the point is already at the top left.
        break;
    case AnchorType::TOP_CENTER:
        anchorX += width / 2;
        break;
    case AnchorType::TOP_RIGHT:
        anchorX += width;
        break;
    case AnchorType::CENTER_LEFT:
        anchorY += height / 2;
        break;
    case AnchorType::CENTER:
        anchorX += width / 2;
        anchorY += height / 2;
        break;
    case AnchorType::CENTER_RIGHT:
        anchorX += width;
        anchorY += height / 2;
        break;
    case AnchorType::BOTTOM_LEFT:
        anchorY += height;
        break;
    case AnchorType::BOTTOM_CENTER:
        anchorX += width / 2;
        anchorY += height;
        break;
    case AnchorType::BOTTOM_RIGHT:
        anchorX += width;
        anchorY += height;
        break;
    }

    ReflectFields->AnchorPoint = POINT{anchorX, anchorY};
}

void AnchorPanelSlot::UpdateOffsetPoint()
{
    const float pivotX        = ReflectFields->PivotX;
    const float pivotY        = ReflectFields->PivotY;

    LONG siblingWidth  = 0;
    LONG siblingHeight = 0;

    std::vector<PlacementUIComponent*> components = gameObject->GetComponents<PlacementUIComponent>();
    std::ranges::for_each(
        components | std::views::filter([this](const PlacementUIComponent* component) { return component != this; }),
        [&siblingWidth, &siblingHeight](const PlacementUIComponent* component) {
            siblingWidth  = std::max(siblingWidth, component->GetSize().cx);
            siblingHeight = std::max(siblingHeight, component->GetSize().cy);
        });

    const LONG offsetX = static_cast<LONG>(std::lerp(0, static_cast<float>(siblingWidth), pivotX));
    const LONG offsetY = static_cast<LONG>(std::lerp(0, static_cast<float>(siblingHeight), pivotY));

    ReflectFields->OffsetPoint = POINT{offsetX, offsetY};
    ReflectFields->SiblingSize = SIZE{siblingWidth, siblingHeight};
}