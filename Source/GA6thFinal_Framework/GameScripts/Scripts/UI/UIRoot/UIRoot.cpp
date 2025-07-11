#include "pchScripts.h"
#include "UIRoot.h"

UIRoot::UIRoot()
{
    if (const ImGuiViewport* viewport = ImGui::GetMainViewport(); viewport != nullptr)
    {
        const ImVec2 size = viewport->Size;
        _size.cx          = static_cast<LONG>(size.x);
        _size.cy          = static_cast<LONG>(size.y);
    }
}

void UIRoot::OnDrawDebug()
{
    UIComponent::OnDrawDebug();
    UmDebugDrawCore.Draw("Editor", {{0, 0}, _size});
}

void UIRoot::OnDrawDebugSelected()
{
    UIComponent::OnDrawDebugSelected();
    UmDebugDrawCore.Draw("Editor", {{0, 0}, _size}, DirectX::Colors::Yellow);
}

void UIRoot::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);
    childGameObject->AddComponent<UIRootSlot>();
}

UIRootSlot::UIRootSlot() = default;