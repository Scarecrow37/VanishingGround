#include "pchScripts.h"
#include "PlacementUIComponent.h"

#include "UI/UIRoot/UIRoot.h"

PlacementUIComponent::PlacementUIComponent() = default;

POINT PlacementUIComponent::GetPoint() const
{
    return ReflectFields->Point;
}

SIZE PlacementUIComponent::GetSize() const
{
    return ReflectFields->Size;
}

POINT PlacementUIComponent::GetScopePoint() const
{
    return ReflectFields->ScopePoint;
}

SIZE PlacementUIComponent::GetScopeSize() const
{
    return ReflectFields->ScopeSize;
}

POINT PlacementUIComponent::GetAbsolutePoint() const
{
    return POINT{.x = ReflectFields->Point.x + ReflectFields->ScopePoint.x,
                 .y = ReflectFields->Point.y + ReflectFields->ScopePoint.y};
}

void PlacementUIComponent::SetScopePlacement(const POINT point, const SIZE size)
{
    ReflectFields->ScopePoint = point;
    ReflectFields->ScopeSize  = size;
    ResetPlacement();
}

int PlacementUIComponent::SortViewOrder(int startOrder)
{
    SetViewOrder(startOrder++);
    const int childCount = transform->GetChildCount();
    for (int i = childCount - 1; i >= 0; --i)
    {
        const Transform* child          = transform->GetChild(i);
        GameObject&      gameObject     = child->gameObject;
        const size_t     componentCount = gameObject.GetComponentCount();
        for (size_t j = 0; j < componentCount; ++j)
        {
            if (PlacementUIComponent* component = gameObject.GetComponentAtIndex<PlacementUIComponent>(j))
            {
                startOrder = component->SortViewOrder(startOrder);
            }
        }
    }
    return startOrder;
}

void PlacementUIComponent::ResetPlacement()
{
    ReflectFields->Point = POINT{0, 0};
    ReflectFields->Size  = ReflectFields->ScopeSize;
    OnPlacementChange();
}

void PlacementUIComponent::OnDetachParent(GameObject* previousParentGameObject)
{
    UIComponent::OnDetachParent(previousParentGameObject);
    POINT newPoint = ReflectFields->Point;
    newPoint.x += ReflectFields->ScopePoint.x;
    newPoint.y += ReflectFields->ScopePoint.y;
    ReflectFields->Point      = newPoint;
    ReflectFields->ScopePoint = POINT{0, 0};
    ReflectFields->ScopeSize  = SIZE{0, 0};
    if (nullptr != previousParentGameObject)
    {
        const Transform& transform = previousParentGameObject->transform;
        RequestViewOrder(transform);
    }
}

void PlacementUIComponent::SetViewOrder(const int viewOrder)
{
    ReflectFields->ViewOrder = viewOrder;
}

float PlacementUIComponent::GetZOrder() const
{
    return static_cast<float>(ReflectFields->ViewOrder);
}

void PlacementUIComponent::RequestViewOrder(const Transform& transform)
{
    UIRoot* uiRoot = nullptr;

    const GameObject& rootObject = transform.gameObject;
    uiRoot                       = rootObject.GetComponent<UIRoot>();

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
        uiRoot->SortViewOrder(1);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"UI Component는 UIRoot의 하위에 있어야 합니다.");
    }
}

void PlacementUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();
    ImGui::Text("%d x %d", ReflectFields->ScopePoint.x, ReflectFields->ScopePoint.y);
    ImGui::SameLine();
    ImGui::Text("Scope Point");

    ImGui::Text("%d x %d", ReflectFields->ScopeSize.cx, ReflectFields->ScopeSize.cy);
    ImGui::SameLine();
    ImGui::Text("Scope Size");

    ImGui::Text("%d", ReflectFields->ViewOrder);
    ImGui::SameLine();
    ImGui::Text("View Order");
}

void PlacementUIComponent::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);
    const Transform& transform = this->transform;
    RequestViewOrder(transform);
}