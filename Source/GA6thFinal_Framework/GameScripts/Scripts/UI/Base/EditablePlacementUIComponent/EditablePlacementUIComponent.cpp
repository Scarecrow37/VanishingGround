#include "pchScripts.h"
#include "EditablePlacementUIComponent.h"

EditablePlacementUIComponent::EditablePlacementUIComponent() = default;

void EditablePlacementUIComponent::DrawDebug()
{
    UIComponent::DrawDebug();
    POINT point = ReflectFields->Basefields.get().Point;
    const auto [scopeX, scopeY] = ReflectFields->Basefields.get().ScopePoint;
    point.x += scopeX;
    point.y += scopeY;
    const SIZE size = ReflectFields->Basefields.get().Size;

    UmDebugDrawCore.Draw("Editor", {point, size}, DirectX::Colors::White);
}

void EditablePlacementUIComponent::DrawDebugSelected()
{
    UIComponent::DrawDebugSelected();
    POINT point = ReflectFields->Basefields.get().Point;
    SIZE  size  = ReflectFields->Basefields.get().Size;

    const auto [scopeX, scopeY] = ReflectFields->Basefields.get().ScopePoint;
    point.x += scopeX;
    point.y += scopeY;
    UmDebugDrawCore.Draw("Editor", {point, size}, DirectX::Colors::Yellow);

    POINT innerPoint{.x = point.x + 1, .y = point.y + 1};
    SIZE  innerSize{.cx = size.cx - 2, .cy = size.cy - 2};
    UmDebugDrawCore.Draw("Editor", {innerPoint, innerSize}, DirectX::Colors::Yellow);

    POINT outerPoint{.x = point.x - 1, .y = point.y - 1};
    SIZE  outerSize{.cx = size.cx + 2, .cy = size.cy + 2};
    UmDebugDrawCore.Draw("Editor", {outerPoint, outerSize}, DirectX::Colors::Yellow);
}

void EditablePlacementUIComponent::ImGuiDrawPropertysEvent()
{
    PlacementUIComponent::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Reset Placement"))
    {
        ResetPlacement();
    }
}