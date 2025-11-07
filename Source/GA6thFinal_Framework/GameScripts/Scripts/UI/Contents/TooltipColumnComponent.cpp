#include "pchScripts.h"
#include "TooltipColumnComponent.h"
#include "Scripts/UI/Contents/TooltipComponent.h"

UMREAL_COMPONENT(TooltipColumnComponent)

TooltipColumnComponent::TooltipColumnComponent() = default;

bool TooltipColumnComponent::IsFull() const
{
    return std::ranges::all_of(_tooltips, [](const TooltipAndActive& tooltip) { return tooltip.IsActive; });
}

void TooltipColumnComponent::Show(const Tooltip::TooltipData& data)
{
    if (const auto inactiveTooltipIterator =
            std::ranges::find_if(_tooltips, [](const TooltipAndActive& tooltip) { return !tooltip.IsActive; });
        inactiveTooltipIterator != _tooltips.end())
    {
        if (const auto tooltipComponent = inactiveTooltipIterator->Component.lock())
        {
            tooltipComponent->SetTooltip(data);
            tooltipComponent->Show();
            inactiveTooltipIterator->IsActive = true;
        }
    }
}

void TooltipColumnComponent::Hide()
{
    std::ranges::for_each(_tooltips, [](TooltipAndActive& tooltip) {
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

void TooltipColumnComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    static Tooltip::TooltipData data = {};
    ImGui::InputInt("Image Asset Id", &data.ImageAssetId);
    ImGui::InputText("Title", &data.Title);
    ImGui::InputText("Description", &data.Description);

    if (ImGui::Button("Show Tooltip"))
    {
        Show(data);
    }

    if (ImGui::Button("Hide Tooltip"))
    {
        Hide();
    }
}

void TooltipColumnComponent::FindComponent()
{
    _tooltips.reserve(MAX_TOOLTIP_COUNT);

    Transform::ForeachBFS(transform, [this](const Transform* childTransform, const int depth) {
        if (depth == 1 && _tooltips.size() < MAX_TOOLTIP_COUNT)
        {
            if (const TooltipComponent* tooltipComponent =
                    childTransform->gameObject->GetComponentDynamic<TooltipComponent>())
            {
                const TooltipAndActive tooltip{.Component = tooltipComponent->GetWeakPtrAs<TooltipComponent>(),
                                               .IsActive  = false};
                _tooltips.push_back(tooltip);
            }
        }
    });
}