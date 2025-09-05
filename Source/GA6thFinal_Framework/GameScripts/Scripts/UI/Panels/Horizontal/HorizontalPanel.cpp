#include "pchScripts.h"
#include "HorizontalPanel.h"
#include <numeric>

HorizontalPanel::HorizontalPanel() = default;

void HorizontalPanel::OnAttachChild(GameObject* childGameObject)
{
    UIComponent::OnAttachChild(childGameObject);

   [[maybe_unused]] auto& slot = childGameObject->AddComponent<HorizontalPanelSlot>();
}

SIZE HorizontalPanel::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;

    SIZE desiredSize = MinSize()(availableSize, _requestedSize, verticalFillMode == FillMode::FILL,
                                 horizontalFillMode == FillMode::FILL);

    const PADDING padding               = Padding;
    const SIZE    childrenAvailableSize = desiredSize - padding.Size();

    const std::vector<UIComponent*> children = Children;
    SIZE                            childrenDesiredSize{};

    LONG stretchCount = 0;

    std::ranges::for_each(children, [childrenAvailableSize, &childrenDesiredSize, &stretchCount](UIComponent* child) {
        if (const HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
        {
            if (const bool isStretch = slot->IsStretch; false == isStretch)
            {
                const SIZE childAvailableSize{.cx = childrenAvailableSize.cx - childrenDesiredSize.cx,
                                              .cy = childrenAvailableSize.cy};
                child->Measure(childAvailableSize);
                const SIZE childDesiredSize = child->DesiredSize;
                childrenDesiredSize.cx += childDesiredSize.cx;
                childrenDesiredSize.cy = std::max(childrenDesiredSize.cy, childDesiredSize.cy);
            }
            else
            {
                ++stretchCount;
            }
        }
    });

    if (stretchCount > 0)
    {
        const LONG stretchedWidth = (childrenAvailableSize.cx - childrenDesiredSize.cx) / stretchCount;

        std::ranges::for_each(
            children, [childrenAvailableSize, &childrenDesiredSize, stretchedWidth](UIComponent* child) {
            if (const HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
            {
                if (const bool isStretch = slot->IsStretch; true == isStretch)
                {
                    const SIZE childAvailableSize{.cx = stretchedWidth, .cy = childrenAvailableSize.cy};
                    child->Measure(childAvailableSize);
                    const SIZE childDesiredSize = child->DesiredSize;
                    childrenDesiredSize.cx += childDesiredSize.cx;
                    childrenDesiredSize.cy = std::max(childrenDesiredSize.cy, childDesiredSize.cy);
                }
            }
        });
    }

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = childrenDesiredSize.cx + padding.Horizontal();
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = childrenDesiredSize.cy + padding.Vertical();

    return desiredSize;
}

SIZE HorizontalPanel::ArrangeOverride(const SIZE finalSize)
{
    const SIZE    desiredSize           = DesiredSize;
    const SIZE    actualSize            = MinSize()(finalSize, desiredSize);
    const PADDING padding               = Padding;
    const SIZE    childrenAvailableSize = actualSize - padding.Size();

    std::vector<UIComponent*> children = Children;

    LONG stretchCount = 0;
    SIZE childrenDesiredSize{};

    std::ranges::for_each(children, [&stretchCount, &childrenDesiredSize](const UIComponent* child) {
        if (const HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
        {
            if (const bool isStretch = slot->IsStretch; true == isStretch)
            {
                ++stretchCount;
            }

            const SIZE childDesiredSize = child->DesiredSize;
            childrenDesiredSize.cx += childDesiredSize.cx;
            childrenDesiredSize.cy = std::max(childrenDesiredSize.cy, childDesiredSize.cy);
        }
    });

    const HorizontalAlignment horizontalAlign = HorizontalAlign;
    const VerticalAlignment   verticalAlign   = VerticalAlign;
    LONG alignPositionX = AlignPoint()(horizontalAlign, childrenAvailableSize.cx - childrenDesiredSize.cx);
    if (stretchCount > 0)
        alignPositionX = 0;

    LONG childrenDesiredWidth = 0;

    std::ranges::for_each(children, [this, childrenAvailableSize, verticalAlign, &childrenDesiredWidth, alignPositionX](UIComponent* child) {
        if (const HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
        {
            const SIZE childAvailableSize = child->DesiredSize;

            const POINT absolutePosition = AbsoluteChildPosition;
            const LONG  alignPositionY = AlignPoint()(verticalAlign, childrenAvailableSize.cy - childAvailableSize.cy);
            const POINT alignPoint{.x = alignPositionX + childrenDesiredWidth, .y = alignPositionY};
            const POINT childPosition = absolutePosition + alignPoint;

            child->Arrange(childPosition, childAvailableSize);

            childrenDesiredWidth += childAvailableSize.cx;
        }
    });

    return actualSize;
}

std::vector<HorizontalPanelSlot*> HorizontalPanel::GetSlots(const GameObject& parentGameObject)
{
    return parentGameObject.GetComponents<HorizontalPanelSlot>();
}

HorizontalPanelSlot::HorizontalPanelSlot() = default;