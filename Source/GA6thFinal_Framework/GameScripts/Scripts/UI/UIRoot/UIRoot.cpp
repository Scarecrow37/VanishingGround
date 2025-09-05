#include "pchScripts.h"
#include "UIRoot.h"

#include "UI/Base/DrawUIComponent/DrawUIComponent.h"

UIRoot::UIRoot() = default;

void UIRoot::SortViewOrder() const
{
    int startOrder = 0;

    Transform& transform = this->transform;

    Transform::ForeachPostOrder(transform, [&startOrder](const Transform* dfsTransform) {
        const GameObject& gameObject = dfsTransform->gameObject;

        auto components = gameObject.GetComponents<DrawUIComponent>();

        std::ranges::for_each(components,
                              [&startOrder](DrawUIComponent* component) { component->SetViewOrder(startOrder++); });
    });
}