#include "pchScripts.h"
#include "SlotComponent.h"

REFLECT_FUNCTION(SlotComponent)

SlotComponent::SlotComponent() = default;

void SlotComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIBaseComponent::OnDetachParent(previousParentGameObject);

    gameObject->Destroy(this);
}
