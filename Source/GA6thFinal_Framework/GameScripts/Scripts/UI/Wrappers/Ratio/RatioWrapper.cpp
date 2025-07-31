#include "pchScripts.h"
#include "RatioWrapper.h"

float RatioWrapper::GetRatio() const
{
    return ReflectFields->Ratio;
}

void RatioWrapper::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();

    const auto  [width, height] = GetSize();
    const float ratio    = GetRatio();
    const LONG  adjustedWidth   = static_cast<LONG>(static_cast<float>(height) * ratio);
    const LONG  adjustedHeight  = static_cast<LONG>(static_cast<float>(width) / ratio);
    const SIZE  newSize{.cx = std::min(width, adjustedWidth), .cy = std::min(height, adjustedHeight)};

    ReflectFields->Basefields.get().Basefields.get().Size = newSize;


    Transform& transform = this->transform;
    std::vector<PlacementUIComponent*> components = FindChildComponents<PlacementUIComponent>()(transform);
    std::ranges::for_each(components, [this](PlacementUIComponent* component) { AssignChild(*component); });
}

void RatioWrapper::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);

    std::vector<PlacementUIComponent*> components = childGameObject->GetComponents<PlacementUIComponent>();
    std::ranges::for_each(components, [this](PlacementUIComponent* component) { AssignChild(*component); });
}

void RatioWrapper::AssignChild(PlacementUIComponent& component) const
{
    const POINT point = GetAbsolutePoint();
    const SIZE  size  = GetSize();
    component.SetScopePlacement(point, size);
}