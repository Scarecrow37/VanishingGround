#include "pchScripts.h"
#include "UIComponent.h"

UIComponent::UIComponent() : Component(TYPE::UI) {}

void UIComponent::OnDrawDebug()
{
    Component::OnDrawDebug();
    const bool isEnable = EnableInHierarchy;
    const bool isActive = gameObject->ActiveInHierarchy;
    if (const bool isDraw = isEnable && isActive; !isDraw)
        return;
    DrawDebug();
}

void UIComponent::OnDrawDebugSelected()
{
    Component::OnDrawDebugSelected();
    const bool isEnable = EnableInHierarchy;
    const bool isActive = gameObject->ActiveInHierarchy;
    if (const bool isDraw = isEnable && isActive; !isDraw)
        return;
    DrawDebugSelected();
}