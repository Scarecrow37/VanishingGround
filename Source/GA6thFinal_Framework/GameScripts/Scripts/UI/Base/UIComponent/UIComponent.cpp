#include "pchScripts.h"
#include "UIComponent.h"

UIComponent::UIComponent() : Component(TYPE::UI) {}

void UIComponent::OnDrawDebug()
{
    Component::OnDrawDebug();
    if (const bool isEnable = EnableInHierarchy; !isEnable)
        return;
    DrawDebug();
}

void UIComponent::OnDrawDebugSelected()
{
    Component::OnDrawDebugSelected();
    if (const bool isEnable = EnableInHierarchy; !isEnable)
        return;
    DrawDebugSelected();
}