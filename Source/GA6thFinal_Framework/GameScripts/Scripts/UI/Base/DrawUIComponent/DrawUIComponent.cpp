#include "pchScripts.h"
#include "DrawUIComponent.h"

#include "UI/UIRoot/UIRoot.h"

void DrawUIComponent::RequestViewOrder(const Transform& transform)
{
    UIRoot* uiRoot = nullptr;

    // transform이 루트라면
    const GameObject& object = transform.gameObject;
    uiRoot                   = object.GetComponent<UIRoot>();

    // transform이 루트가 아니라면
    if (nullptr == uiRoot)
    {
        if (const Transform* root = transform.Root; nullptr != root)
        {
            const GameObject& rootObject = root->gameObject;
            uiRoot                       = rootObject.GetComponent<UIRoot>();
        }
    }

    if (nullptr != uiRoot)
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

        ImGui::Text("%d", viewOrder);
        ImGui::SameLine();
        ImGui::Text("View Order");
    }
}

void DrawUIComponent::OnAttachChild(GameObject* childGameObject)
{
    PlacementUIComponent::OnAttachChild(childGameObject);

    const Transform& transform = this->transform;
    RequestViewOrder(transform);
}

float DrawUIComponent::GetZOrder() const
{
    return static_cast<float>(ReflectFields->ViewOrder);
}