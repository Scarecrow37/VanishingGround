#include "pchScripts.h"
#include "UIRoot.h"

#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

UIRoot::UIRoot()
{
    GetSizeFromViewport();
}

void UIRoot::OnAttachChild(GameObject* childGameObject)
{
    PlacementUIComponent::OnAttachChild(childGameObject);
    UIRootSlot& slot = childGameObject->AddComponent<UIRootSlot>();
    AssignChild(slot);
}

void UIRoot::OnDetachParent(GameObject* previousParentGameObject)
{
    PlacementUIComponent::OnDetachParent(previousParentGameObject);
    ReflectFields->Basefields.get().Point = {0, 0};
    GetSizeFromViewport();
    OnPlacementChange();
}

void UIRoot::ImGuiDrawPropertysEvent()
{
    PlacementUIComponent::ImGuiDrawPropertysEvent();
    ImGui::Text("%d x %d", ReflectFields->Basefields.get().Point.x, ReflectFields->Basefields.get().Point.y);
    ImGui::SameLine();
    ImGui::Text("Point");
    ImGui::Text("%d x %d", ReflectFields->Basefields.get().Size.cx, ReflectFields->Basefields.get().Size.cy);
    ImGui::SameLine();
    ImGui::Text("Size");
}

void UIRoot::OnPlacementChange()
{
    PlacementUIComponent::OnPlacementChange();

    std::vector<UIRootSlot*> slots = FindChildComponents<UIRootSlot>()(transform);
    std::ranges::for_each(slots, [this](UIRootSlot* slot) { AssignChild(*slot); });
}

void UIRoot::AssignChild(UIRootSlot& slot) const
{
    const POINT point = GetPoint();
    const SIZE  size  = GetSize();
    slot.SetScopePlacement(point, size);
}

void UIRoot::GetSizeFromViewport()
{
    // 민재가 Viewport 크기 가져오는 함수 안말들어 줘서 Magic Number씀.
    ReflectFields->Basefields.get().Size = {1920,1080};
}

UIRootSlot::UIRootSlot() = default;

void UIRootSlot::OnPlacementChange()
{
    PanelSlotComponent::OnPlacementChange();

    const POINT point = GetAbsolutePoint();
    const SIZE  size  = GetSize();
    PassScopedPlacement(point, size);
}