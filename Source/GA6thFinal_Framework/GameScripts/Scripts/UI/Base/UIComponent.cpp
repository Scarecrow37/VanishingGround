#include "pchScripts.h"
#include "UIComponent.h"

UIComponent::UIComponent() : Component(TYPE::UI), _size{100, 100} {}

void UIComponent::OnDrawDebugSelected()
{
    Component::OnDrawDebugSelected();
    
}