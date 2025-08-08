#include "pchScripts.h"
#include "PlacementUIComponent.h"

#include "UI/UIRoot/UIRoot.h"

PlacementUIComponent::PlacementUIComponent() = default;

POINT PlacementUIComponent::GetPoint() const
{
    return ReflectFields->Point;
}

SIZE PlacementUIComponent::GetSize() const
{
    return ReflectFields->Size;
}

POINT PlacementUIComponent::GetScopePoint() const
{
    return ReflectFields->ScopePoint;
}

SIZE PlacementUIComponent::GetScopeSize() const
{
    return ReflectFields->ScopeSize;
}

POINT PlacementUIComponent::GetAbsolutePoint() const
{
    return GetScopePoint() + GetPoint();
}

SIZE PlacementUIComponent::GetContentSize() const
{
    return GetSize();
}

void PlacementUIComponent::SetScopePlacement(const POINT scopePoint, const SIZE scopeSize)
{
    const POINT previousPoint = GetScopePoint();
    const SIZE  previousSize  = GetScopeSize();
    if (previousPoint != scopePoint || previousSize != scopeSize)
    {
        ReflectFields->ScopePoint = scopePoint;
        ReflectFields->ScopeSize  = scopeSize;
        ResetPlacement();
    }
}

void PlacementUIComponent::ResetPlacement()
{
    ReflectFields->Point = POINT{0, 0};
    ReflectFields->Size  = ReflectFields->ScopeSize;
    OnPlacementChange();
}

void PlacementUIComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIComponent::OnDetachParent(previousParentGameObject);
    POINT newPoint = ReflectFields->Point;
    newPoint.x += ReflectFields->ScopePoint.x;
    newPoint.y += ReflectFields->ScopePoint.y;
    ReflectFields->Point      = newPoint;
    ReflectFields->ScopePoint = POINT{0, 0};
    ReflectFields->ScopeSize  = SIZE{0, 0};
}

void PlacementUIComponent::SpreadPlacementToParent()
{
    if (const Transform* parentTransform = transform->Parent; nullptr != parentTransform)
    {
        const GameObject&                  parentGameObject = parentTransform->gameObject;
        std::vector<PlacementUIComponent*> components       = parentGameObject.GetComponents<PlacementUIComponent>();
        std::ranges::for_each(components,
                              [this](PlacementUIComponent* component) { component->OnChildPlacementChange(this); });
    }
}

void PlacementUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        ImGui::Text("%d x %d", ReflectFields->ScopePoint.x, ReflectFields->ScopePoint.y);
        ImGui::SameLine();
        ImGui::Text("Scope Point");

        ImGui::Text("%d x %d", ReflectFields->ScopeSize.cx, ReflectFields->ScopeSize.cy);
        ImGui::SameLine();
        ImGui::Text("Scope Size");
    }
}