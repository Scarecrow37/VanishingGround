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
    ReflectFields->Point = ReflectFields->ScopePoint;
    ReflectFields->Size  = ReflectFields->ScopeSize;
}

void PlacementUIComponent::OnDrawDebug()
{
    UIComponent::OnDrawDebug();
    POINT point                 = ReflectFields->Point;
    const auto [scopeX, scopeY] = ReflectFields->ScopePoint;
    point.x += scopeX;
    point.y += scopeY;
    const SIZE size = ReflectFields->Size;

    UmDebugDrawCore.Draw("Editor", {point, size});
}

void PlacementUIComponent::OnDrawDebugSelected()
{
    UIComponent::OnDrawDebugSelected();
    POINT point                 = ReflectFields->Point;
    const auto [scopeX, scopeY] = ReflectFields->ScopePoint;
    point.x += scopeX;
    point.y += scopeY;
    SIZE size = ReflectFields->Size;

    UmDebugDrawCore.Draw("Editor", {point, size}, DirectX::Colors::Yellow);

    point.x += 1;
    point.y += 1;
    size.cx -= 2;
    size.cy -= 2;
    UmDebugDrawCore.Draw("Editor", {point, size}, DirectX::Colors::Yellow);
}

void PlacementUIComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIComponent::OnDetachParent(previousParentGameObject);
    POINT newPoint = ReflectFields->Point;
    newPoint.x += ReflectFields->ScopePoint.x;
    newPoint.y += ReflectFields->ScopePoint.y;
    ReflectFields->Point      = newPoint;
    ReflectFields->ScopePoint = {0, 0};
    ReflectFields->ScopeSize  = {0, 0};
}