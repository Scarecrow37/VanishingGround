#include "pchScripts.h"
#include "AreaUIComponent.h"

AreaUIComponent::AreaUIComponent() = default;

void AreaUIComponent::OnDrawDebug()
{
    UIComponent::OnDrawDebug();
    const POINT point = Point;
    const SIZE size = Size;
    UmDebugDrawCore.Draw("Editor", {point, size});
}

void AreaUIComponent::OnDrawDebugSelected()
{
    UIComponent::OnDrawDebugSelected();
    const POINT point = Point;
    const SIZE  size  = Size;
    UmDebugDrawCore.Draw("Editor", {point, size}, DirectX::Colors::Yellow);
}

void AreaUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Reset Area"))
    {
        ResetArea();
    }
}

