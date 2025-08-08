#include "pchScripts.h"
#include "UIComponent.h"

bool UIComponent::_isDebug = false;

UIComponent::UIComponent() : Component(TYPE::UI) {}

void UIComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Debug"))
    {
        _isDebug = !_isDebug;
    }
}

void UIComponent::OnDrawDebug()
{
    Component::OnDrawDebug();

    if (const bool isEnable = EnableInHierarchy; isEnable)
    {
        OnDrawDebugOverride();
    }
}

void UIComponent::OnDrawDebugSelected()
{
    Component::OnDrawDebugSelected();

    if (const bool isEnable = EnableInHierarchy; isEnable)
    {
        OnDrawDebugSelectedOverride();
    }
}