#include "pchScripts.h"
#include "DrawUIComponent.h"


void DrawUIComponent::RequestViewOrder() const
{
    if (const UIRoot* uiRoot = this->Root; nullptr != uiRoot)
    {
        uiRoot->SortViewOrder();
    }
    else if (false == Global::IsPlay())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Component는 UIRoot의 하위에 있어야 합니다.");
    }
}

UIRoot* DrawUIComponent::GetRoot(const GameObject& rootGameObject)
{
    return rootGameObject.GetComponent<UIRoot>();
}

DrawUIComponent::DrawUIComponent() = default;

void DrawUIComponent::SetViewOrder(const int viewOrder)
{
    ReflectFields->ViewOrder = viewOrder;
}

void DrawUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        const int viewOrder = ReflectFields->ViewOrder;
        ImGuiDebug()("View Order", viewOrder);
    }
}

float DrawUIComponent::GetZOrder() const
{
    return static_cast<float>(ReflectFields->ViewOrder);
}

void DrawUIComponent::OnAttachParent(GameObject* parentGameObject)
{
    UIComponent::OnAttachParent(parentGameObject);

    RequestViewOrder();
}