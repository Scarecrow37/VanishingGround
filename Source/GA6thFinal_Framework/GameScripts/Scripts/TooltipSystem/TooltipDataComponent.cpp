#include "pchScripts.h"
#include "TooltipDataComponent.h"

UMREAL_COMPONENT(TooltipDataComponent)

TooltipDataComponent::TooltipDataComponent() = default;

void TooltipDataComponent::SetTooltips(const std::initializer_list<int> ids)
{
    _tooltipIds = std::vector(ids);
}

std::span<const int> TooltipDataComponent::GetTooltips() const
{
    return _tooltipIds;
}

void TooltipDataComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    ImGui::Text("Tooltip IDs:");
    for (const int id : _tooltipIds)
    {
        ImGui::Text("- %d", id);
    }
}