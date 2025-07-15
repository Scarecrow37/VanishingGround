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
    AddSlot(childGameObject);
}

void UIRoot::AddSlot(GameObject* gameObject) const
{
    UIRootSlot& slot = gameObject->AddComponent<UIRootSlot>();
    slot.SetPlacement(ReflectFields->Basefields.get().Point, ReflectFields->Basefields.get().Size);
}

UIRootSlot::UIRootSlot() = default;

void UIRootSlot::PassPlacement() const
{
    PanelSlotComponent::PassPlacement();
    for (int i = 0; i < gameObject->GetComponentCount(); ++i)
    {
        if (PlacementUIComponent* areaComponent = gameObject->GetComponentAtIndex<PlacementUIComponent>(i))
        {
            areaComponent->SetScopePlacement(ReflectFields->Basefields.get().Point,
                                             ReflectFields->Basefields.get().Size);
            break;
        }
    }
}