#include "pchScripts.h"
#include "DrawUIComponent.h"

#include "UI/UIRoot/UIRoot.h"

void DrawUIComponent::RequestViewOrder() const
{
    if (const UIRoot* uiRoot = this->uiRoot; nullptr != uiRoot)
    {
        uiRoot->SortViewOrder();
    }
    else if (false == Global::IsPlay())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Component는 UIRoot의 하위에 있어야 합니다.");
    }
}

DrawUIComponent::DrawUIComponent() = default;

void DrawUIComponent::SetViewOrder(const int viewOrder)
{
    ReflectFields->ViewOrder = viewOrder;
}

void DrawUIComponent::ImGuiDrawPropertysEvent()
{
    PlacementUIComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        const int viewOrder = ReflectFields->ViewOrder;
        DrawDebug()("View Order", viewOrder);
    }
}

void DrawUIComponent::OnAttachChild(GameObject* childGameObject)
{
    PlacementUIComponent::OnAttachChild(childGameObject);

    RequestViewOrder();
}

float DrawUIComponent::GetZOrder() const
{
    return static_cast<float>(ReflectFields->ViewOrder);
}