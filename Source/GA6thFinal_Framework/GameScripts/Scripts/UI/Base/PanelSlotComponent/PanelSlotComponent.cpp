#include "pchScripts.h"
#include "PanelSlotComponent.h"

PanelSlotComponent::PanelSlotComponent() = default;

void PanelSlotComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIComponent::OnDetachParent(previousParentGameObject);
    gameObject->Destroy(this);
}