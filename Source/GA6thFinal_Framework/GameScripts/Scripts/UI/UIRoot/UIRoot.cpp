#include "pchScripts.h"
#include "UIRoot.h"

#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

UIRoot::UIRoot()
{
    GetSizeFromViewport();
}

void UIRoot::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);
    UIRootSlot& slot = childGameObject->AddComponent<UIRootSlot>();
    slot.SetPlacement(ReflectFields->Basefields.get().Point, ReflectFields->Basefields.get().Size);
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
    const int childCount = transform->GetChildCount();
    for (int i = 0; i < childCount; ++i)
    {
        const Transform* child      = transform->GetChild(i);
        GameObject&      gameObject = child->gameObject;
        const size_t componentCount    = gameObject.GetComponentCount();
        for (size_t j = 0; j < componentCount; ++j)
        {
            if (UIRootSlot* slot = gameObject.GetComponentAtIndex<UIRootSlot>(j))
            {
                slot->SetPlacement(ReflectFields->Basefields.get().Point, ReflectFields->Basefields.get().Size);
            }
        }
    }
}

void UIRoot::GetSizeFromViewport()
{
    // 민재가 Viewport 크기 가져오는 함수 안말들어 줘서 Magic Number씀.
    ReflectFields->Basefields.get().Size = {1920,1080};
}

UIRootSlot::UIRootSlot() = default;

void UIRootSlot::OnSetPlacement()
{
    PanelSlotComponent::PassScopedPlacement(ReflectFields->Basefields.get().Point, ReflectFields->Basefields.get().Size);
}