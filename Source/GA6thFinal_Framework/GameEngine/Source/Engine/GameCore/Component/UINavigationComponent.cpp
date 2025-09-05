#include "pch.h"
#include "UINavigationComponent.h"

UINavigationComponent::UINavigationComponent() = default;

void UINavigationComponent::OnDrawDebugSelectedOverride()
{
    UIBaseComponent::OnDrawDebugSelectedOverride();

    // TODO Draw Navigation Route
}