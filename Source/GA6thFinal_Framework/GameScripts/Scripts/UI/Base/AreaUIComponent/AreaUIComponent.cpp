#include "pchScripts.h"
#include "AreaUIComponent.h"

AreaUIComponent::AreaUIComponent() : _point{0, 0}, _size{200, 100}
{
}

void AreaUIComponent::OnDrawDebug()
{
    UIComponent::OnDrawDebug();
    UmDebugDrawCore.Draw("Editor", {_point, _size});
}

void AreaUIComponent::OnDrawDebugSelected()
{
    UIComponent::OnDrawDebugSelected();
    UmDebugDrawCore.Draw("Editor", {_point, _size}, DirectX::Colors::Yellow);
}

void AreaUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Reset Area"))
    {
        ResetArea();
    }
}

