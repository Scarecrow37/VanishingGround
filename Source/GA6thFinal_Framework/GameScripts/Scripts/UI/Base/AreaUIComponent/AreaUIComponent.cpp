#include "pchScripts.h"
#include "AreaUIComponent.h"

AreaUIComponent::AreaUIComponent() = default;

void AreaUIComponent::OnDrawDebug()
{
    UIComponent::OnDrawDebug();
    DrawDebugQuad(DirectX::Colors::White);
}

void AreaUIComponent::OnDrawDebugSelected()
{
    UIComponent::OnDrawDebugSelected();
    DrawDebugQuad(DirectX::Colors::Yellow);
}

void AreaUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Reset Area"))
    {
        ResetArea();
    }
}

void AreaUIComponent::DrawDebugQuad(FXMVECTOR color) const
{
    POINT point = Point;
    if (_scopePoint)
    {
        point.x = _scopePoint->x;
        point.y = _scopePoint->y;
    }
    SIZE  size  = Size;
    if (_scopeSize)
    {
        size.cx = _scopeSize->cx;
        size.cy = _scopeSize->cy;
    }
    UmDebugDrawCore.Draw("Editor", {point, size}, color);
}

