#include "pchScripts.h"
#include "TooltipColumnComponent.h"
#include "Scripts/UI/Contents/TooltipComponent.h"

UMREAL_COMPONENT(TooltipColumnComponent)

TooltipColumnComponent::TooltipColumnComponent() = default;

bool TooltipColumnComponent::IsFull() const
{
    return std::ranges::all_of(_tooltips, [](const Tooltip& tooltip) { return tooltip.IsActive; });
}

void TooltipColumnComponent::Hide()
{
    std::ranges::for_each(_tooltips, [](Tooltip& tooltip) {
        if (const auto tooltipComponent = tooltip.Component.lock())
        {
            tooltipComponent->Hide();
            tooltip.IsActive = false;
        }
    });
}

void TooltipColumnComponent::Awake()
{
    Component::Awake();

    FindComponent();
}

void TooltipColumnComponent::FindComponent()
{
    _tooltips.reserve(4);

    Transform::ForeachBFS(transform, [this](const Transform* childTransform, const int depth) {
        if (depth == 1)
        {
            if (const TooltipComponent* tooltipComponent =
                    childTransform->gameObject->GetComponentDynamic<TooltipComponent>())
            {
                const Tooltip tooltip{.Component = tooltipComponent->GetWeakPtrAs<TooltipComponent>(), .IsActive = false};
                _tooltips.push_back(tooltip);
            }
        }
    });
}