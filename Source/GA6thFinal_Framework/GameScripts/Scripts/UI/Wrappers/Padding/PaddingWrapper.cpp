#include "pchScripts.h"
#include "PaddingWrapper.h"

POINT PaddingWrapper::GetPaddedPoint() const
{
    const auto [x, y] = GetAbsolutePoint();
    const POINT paddedPoint{.x = x + ReflectFields->PadLeft, .y = y + ReflectFields->PadTop};
    return paddedPoint;
}

SIZE PaddingWrapper::GetPaddedSize() const
{
    const auto [width, height] = GetSize();
    const SIZE paddedSize{.cx = width - (ReflectFields->PadLeft + ReflectFields->PadRight),
                          .cy = height - (ReflectFields->PadTop + ReflectFields->PadBottom)};
    return paddedSize;
}

void PaddingWrapper::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();

    Transform&                         transform  = this->transform;
    std::vector<PlacementUIComponent*> components = FindChildComponents<PlacementUIComponent>()(transform);
    std::ranges::for_each(components, [this](PlacementUIComponent* component) { AssignChild(*component); });
}

void PaddingWrapper::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);

    std::vector<PlacementUIComponent*> components = FindComponents<PlacementUIComponent>()(*childGameObject);
    std::ranges::for_each(components, [this](PlacementUIComponent* component) { AssignChild(*component); });
}

void PaddingWrapper::AssignChild(PlacementUIComponent& component) const
{
    const POINT point = GetPaddedPoint();
    const SIZE  size  = GetPaddedSize();
    component.SetScopePlacement(point, size);
}