#include "pchScripts.h"
#include "PanelSlotComponent.h"

PanelSlotComponent::PanelSlotComponent() = default;

void PanelSlotComponent::PassScopedPlacement(const POINT point, const SIZE size) const
{
    std::vector<PlacementUIComponent*> components = FindComponents<PlacementUIComponent>()(gameObject);
    std::ranges::for_each(
        components, [point, size](PlacementUIComponent* component) { component->SetScopePlacement(point, size); });
}

void PanelSlotComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIComponent::OnDetachParent(previousParentGameObject);
    gameObject->Destroy(this);
}
