#include "pch.h"
#include "UIBaseComponent.h"

REFLECT_FUNCTION(UIBaseComponent)

bool UIBaseComponent::_isDebug = false;

UIBaseComponent::UIBaseComponent() : Component(TYPE::UI)
{
    
}

void UIBaseComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Debug"))
    {
        _isDebug = !_isDebug;
    }
}

void UIBaseComponent::OnDrawDebug()
{
    Component::OnDrawDebug();

    if (const bool isEnable = EnableInHierarchy; isEnable)
    {
        OnDrawDebugOverride();
    }
}

void UIBaseComponent::OnDrawDebugSelected()
{
    Component::OnDrawDebugSelected();

    if (const bool isEnable = EnableInHierarchy; isEnable)
    {
        OnDrawDebugSelectedOverride();
    }
}