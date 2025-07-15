#include "pchScripts.h"
#include "PanelSlotComponent.h"

PanelSlotComponent::PanelSlotComponent() = default;

void PanelSlotComponent::SetPlacement(const POINT point, const SIZE size)
{
    ReflectFields->Basefields.get().Point = point;
    ReflectFields->Basefields.get().Size  = size;
    PassPlacement();
}

void PanelSlotComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIComponent::OnDetachParent(previousParentGameObject);
    gameObject->Destroy(this);
}