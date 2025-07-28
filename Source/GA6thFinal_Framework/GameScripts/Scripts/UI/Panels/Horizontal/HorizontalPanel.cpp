#include "pchScripts.h"
#include "HorizontalPanel.h"
#include <numeric>

HorizontalPanel::HorizontalPanel() = default;

void HorizontalPanel::OnAttachChild(GameObject* childGameObject)
{
    EditablePlacementUIComponent::OnAttachChild(childGameObject);

   [[maybe_unused]] auto& slot = childGameObject->AddComponent<HorizontalPanelSlot>();
    OnPlacementChange();
}

void HorizontalPanel::OnChildPlacementChange(PlacementUIComponent* changedComponent)
{
    EditablePlacementUIComponent::OnChildPlacementChange(changedComponent);

    OnPlacementChange();
}

void HorizontalPanel::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();

    std::vector<HorizontalPanelSlot*> slots = FindChildComponents<HorizontalPanelSlot>()(transform);
    const size_t                      stretchCount = 
        std::ranges::count_if(slots, [](const HorizontalPanelSlot* slot) { return slot->IsStretch; });
    auto noneStretchWidths =
        slots | std::views::filter([](const HorizontalPanelSlot* slot) { return !slot->IsStretch; }) |
        std::views::transform([](const HorizontalPanelSlot* slot) { return slot->GetSiblingWidth(); });
    const LONG noneStretchWidthSum = std::accumulate(noneStretchWidths.begin(), noneStretchWidths.end(), 0);
    const auto [width, height]     = GetSize();
    const LONG stretchWidth        = stretchCount > 0 ? (width - noneStretchWidthSum) / static_cast<LONG>(stretchCount) : 0;

    int  index  = 0;
    LONG offset = 0;
    std::ranges::for_each(slots, [this, &index, &offset, stretchWidth, width](HorizontalPanelSlot* slot) {
        bool       isStretch = slot->IsStretch;
        LONG limit     = 0;
        if (isStretch)
        {
            limit = offset + stretchWidth;
        }
        else
        {
            const LONG siblingWidth = slot->GetSiblingWidth();
            limit                   = offset + siblingWidth;
        }
        limit = std::min(limit, width);
        //const LONG limit = std::min(slot->IsStretch ? offset + stretchWidth : offset + slot->GetSiblingWidth(), width);
        AssignChild(*slot, index++, offset, limit);
        offset = limit;
    });
}

void HorizontalPanel::AssignChild(HorizontalPanelSlot& slot, const unsigned int index, const LONG offset, const LONG limit) const
{
    const auto  [x, y]          = GetAbsolutePoint();
    const auto  [width, height] = GetSize();
    const POINT scopePoint{.x = x + offset, .y = y};
    const SIZE  scopeSize{.cx = limit - offset, .cy = height};

    slot.SetLayout(index, offset, limit);
    slot.SetScopePlacement(scopePoint, scopeSize);
}

void HorizontalPanelSlot::OnPlacementChange()
{
    PanelSlotComponent::OnPlacementChange();

    const POINT point = GetAbsolutePoint();
    const SIZE  size  = GetSize();
    PassScopedPlacementToSibling(point, size);
}

LONG HorizontalPanelSlot::GetLimit() const
{
    return ReflectFields->Limit;
}

LONG HorizontalPanelSlot::GetOffset() const
{
    return ReflectFields->Offset;
}

LONG HorizontalPanelSlot::GetSiblingWidth() const
{
    const GameObject&                  gameObject = this->gameObject;
    std::vector<PlacementUIComponent*> components = gameObject.GetComponents<PlacementUIComponent>();

    LONG width = 0;
    std::ranges::for_each(
        components | std::views::filter([this](const PlacementUIComponent* component) { return component != this; }),
        [&width](const PlacementUIComponent* component) {
            auto [componentWidth, componentHeight] = component->GetSize();
            width                                  = std::max(width, componentWidth);
        });

    return width;
}

void HorizontalPanelSlot::SetLayout(const unsigned int order, const LONG offset, const LONG limit)
{
    ReflectFields->Order  = order;
    ReflectFields->Offset = offset;
    ReflectFields->Limit  = limit;
}