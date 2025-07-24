#include "pchScripts.h"
#include "CenterWrapper.h"


POINT CenterWrapper::GetCenterPoint(const SIZE childSize) const
{
    const auto [absoluteX, absoluteY] = GetAbsolutePoint();
    const auto [width, height]        = GetSize();

    POINT centerPoint{.x = absoluteX, .y = absoluteY};
    if (IsCenterHorizontal())
    {
        centerPoint.x += (width - childSize.cx) / 2;
    }
    if (IsCenterVertical())
    {
        centerPoint.y += (height - childSize.cy) / 2;
    }
    return centerPoint;
}

bool CenterWrapper::IsCenterHorizontal() const
{
    return ReflectFields->CenterType & CENTER_HORIZONTAL;
}

bool CenterWrapper::IsCenterVertical() const
{
    return ReflectFields->CenterType & CENTER_VERTICAL;
}

void CenterWrapper::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();

    AssignChild();
}

void CenterWrapper::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);

    AssignChild();
}

void CenterWrapper::AssignChild() const
{
    Transform&                         transform  = this->transform;
    std::vector<PlacementUIComponent*> components = FindChildComponents<PlacementUIComponent>()(transform);
    std::ranges::for_each(components, [this](PlacementUIComponent* component) {
        const SIZE  childSize = component->GetSize();
        const POINT point     = GetCenterPoint(childSize);
        const SIZE  size      = GetSize();
        component->SetScopePlacement(point, size);
    });
}