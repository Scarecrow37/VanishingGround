#include "pchScripts.h"
#include "SlotComponent.h"

SlotComponent::SlotComponent() = default;

void SlotComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIBaseComponent::OnDetachParent(previousParentGameObject);

    gameObject->Destroy(this);
}
