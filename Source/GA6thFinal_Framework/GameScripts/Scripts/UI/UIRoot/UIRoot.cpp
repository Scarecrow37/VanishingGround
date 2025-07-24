#include "pchScripts.h"
#include "UIRoot.h"

#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

UIRoot::UIRoot()
{
    if (const ImGuiViewport* viewport = ImGui::GetMainViewport(); viewport != nullptr)
    {
        const ImVec2 size = viewport->Size;
        ReflectFields->Basefields.get().Size = SIZE{static_cast<LONG>(size.x), static_cast<LONG>(size.y)};
    }
}

void UIRoot::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);
    UIRootSlot& slot = childGameObject->AddComponent<UIRootSlot>();
    slot.SetPlacement(ReflectFields->Basefields.get().Point, ReflectFields->Basefields.get().Size);
}

UIRootSlot::UIRootSlot() = default;

void UIRootSlot::OnSetPlacement()
{
    PanelSlotComponent::PassScopedPlacement(ReflectFields->Basefields.get().Point, ReflectFields->Basefields.get().Size);
}