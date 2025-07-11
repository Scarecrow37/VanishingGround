#include "pchScripts.h"
#include "UIComponent.h"

UIComponent::UIComponent() : UIComponent(POINT{0, 0}, SIZE{100, 100}) {};

UIComponent::UIComponent(const POINT point, const SIZE size)
    : Component(TYPE::UI), _point(point), _size(size), _isSelected(false)
{
}

void UIComponent::OnDrawDebug()
{
    Component::OnDrawDebug();
    UmDebugDrawCore.Draw("Editor", {_point, _size});
}

void UIComponent::OnDrawDebugSelected()
{
    Component::OnDrawDebugSelected();
    UmDebugDrawCore.Draw("Editor", {_point, _size}, Colors::Yellow);
}