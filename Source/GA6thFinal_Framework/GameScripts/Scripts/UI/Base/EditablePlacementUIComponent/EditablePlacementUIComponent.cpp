#include "pchScripts.h"
#include "EditablePlacementUIComponent.h"

EditablePlacementUIComponent::EditablePlacementUIComponent() = default;

void EditablePlacementUIComponent::ImGuiDrawPropertysEvent()
{
    UIComponent::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Reset Placement"))
    {
        ResetPlacement();
    }
}