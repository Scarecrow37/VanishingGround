#include "pchScripts.h"
#include "PanelSlotComponent.h"

PanelSlotComponent::PanelSlotComponent() = default;

void PanelSlotComponent::SetPlacement(const POINT point, const SIZE size)
{
    ReflectFields->Basefields.get().Point = point;
    ReflectFields->Basefields.get().Size  = size;
    PassPlacement();
}

void PanelSlotComponent::PassPlacement(const POINT point, const SIZE size) const
{
    for (int i = 0; i < gameObject->GetComponentCount(); ++i)
    {
        if (PlacementUIComponent* areaComponent = gameObject->GetComponentAtIndex<PlacementUIComponent>(i))
        {
            areaComponent->SetScopePlacement(point, size);
            break;
        }
    }
}

void PanelSlotComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIComponent::OnDetachParent(previousParentGameObject);
    gameObject->Destroy(this);
}
