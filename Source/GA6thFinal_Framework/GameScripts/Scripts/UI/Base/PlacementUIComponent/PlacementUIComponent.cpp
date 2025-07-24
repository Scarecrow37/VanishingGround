#include "pchScripts.h"
#include "PlacementUIComponent.h"

PlacementUIComponent::PlacementUIComponent() = default;

void PlacementUIComponent::SetScopePlacement(const POINT point, const SIZE size)
{
    ReflectFields->ScopePoint = point;
    ReflectFields->ScopeSize  = size;
    ResetPlacement();
}

void PlacementUIComponent::ResetPlacement()
{
    ReflectFields->Point = POINT{0,0};
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

void PlacementUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();
    ImGui::Text("%d x %d", ReflectFields->ScopePoint.x, ReflectFields->ScopePoint.y);
    ImGui::SameLine();
    ImGui::Text("Scope Point");
    ImGui::Text("%d x %d", ReflectFields->ScopeSize.cx, ReflectFields->ScopeSize.cy);
    ImGui::SameLine();
    ImGui::Text("Scope Size");
}