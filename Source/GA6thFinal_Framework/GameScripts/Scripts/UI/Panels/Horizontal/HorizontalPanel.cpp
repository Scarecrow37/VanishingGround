#include "pchScripts.h"
#include "HorizontalPanel.h"
#include <numeric>


UMREAL_COMPONENT(HorizontalPanel)
UMREAL_COMPONENT(HorizontalPanelSlot)

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

    SIZE desiredSize = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                 verticalFillMode == FillMode::FILL);

    const PADDING padding               = Padding;
    const SIZE    childrenAvailableSize = desiredSize - padding.Size();
    const LONG    space                 = Space;

    const std::vector<HorizontalPanelSlot*> slots = Slots;
    std::ranges::for_each(slots, [](HorizontalPanelSlot* slot) { slot->SetLine(0); });

    const std::vector<UIComponent*> children = Children;

    LONG              currentLine = 0;
    std::vector<std::pair<SIZE, LONG>> childrenDesiredSizePerLine;
    childrenDesiredSizePerLine.push_back(std::make_pair(SIZE{}, 0));

    std::ranges::for_each(children, [childrenAvailableSize, &childrenDesiredSizePerLine, space,
                                     &currentLine](UIComponent* child) {
        if (HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
        {
            if (const bool isStretch = slot->IsStretch; false == isStretch)
            {
                const SIZE childAvailableSize{.cx = childrenAvailableSize.cx, .cy = childrenAvailableSize.cy};
                child->Measure(childAvailableSize);
                const SIZE childDesiredSize = child->DesiredSize;
                if (childrenDesiredSizePerLine[currentLine].first.cx + childDesiredSize.cx + space > childAvailableSize.cx)
                {
                    ++currentLine;
                    childrenDesiredSizePerLine.push_back(std::make_pair(SIZE{}, 0));
                }
                else
                {
                    childrenDesiredSizePerLine[currentLine].first.cx += space;
                }
                childrenDesiredSizePerLine[currentLine].first.cx += childDesiredSize.cx;
                childrenDesiredSizePerLine[currentLine].first.cy =
                    std::max(childrenDesiredSizePerLine[currentLine].first.cy, childDesiredSize.cy);
                slot->SetLine(currentLine);
            }
            else
            {
                ++childrenDesiredSizePerLine[currentLine].second;
            }
        }
    });

    std::vector<LONG> stretchWidths;
    stretchWidths.resize(childrenDesiredSizePerLine.size(), 0);

    for (LONG line = 0; line < static_cast<LONG>(childrenDesiredSizePerLine.size()); ++line)
    {
        const LONG stretchCount = childrenDesiredSizePerLine[line].second;
        if (stretchCount > 0)
        {
            const LONG stretchedWidth = (childrenAvailableSize.cx - childrenDesiredSizePerLine[line].first.cx) / stretchCount;
            stretchWidths[line]      = stretchedWidth;
        }
    }

    std::ranges::for_each(
        children, [childrenAvailableSize, &childrenDesiredSizePerLine, &stretchWidths](UIComponent* child) {
        if (const HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
        {
            if (const bool isStretch = slot->IsStretch; true == isStretch)
            {
                const LONG line           = slot->Line;
                const LONG stretchedWidth = stretchWidths[line];
                const SIZE childAvailableSize{.cx = stretchedWidth, .cy = childrenAvailableSize.cy};
                child->Measure(childAvailableSize);
                const SIZE childDesiredSize = child->DesiredSize;
                childrenDesiredSizePerLine[line].first.cx += childDesiredSize.cx;
                childrenDesiredSizePerLine[line].first.cy =
                    std::max(childrenDesiredSizePerLine[line].first.cy, childDesiredSize.cy);
            }
        }
    });

    SIZE contentSize{};
    for (const auto& [lineWidth, lineHeight] : childrenDesiredSizePerLine | std::views::keys)
    {
        contentSize.cx = std::max(contentSize.cx, lineWidth);
        contentSize.cy += lineHeight;
    }
    contentSize.cy += LineSpace * (static_cast<LONG>(childrenDesiredSizePerLine.size()) - 1);

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = contentSize.cx + padding.Horizontal();
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = contentSize.cy + padding.Vertical();

    return desiredSize;
}

SIZE HorizontalPanel::ArrangeOverride(const SIZE finalSize)
{
    const SIZE    desiredSize           = DesiredSize;
    const SIZE    actualSize            = MinSize()(finalSize, desiredSize);
    const PADDING padding               = Padding;
    const SIZE    childrenAvailableSize = actualSize - padding.Size();

    std::vector<UIComponent*> children = Children;
    LONG                      space    = Space;

    std::vector<SIZE> childrenDesiredSizes;

    std::ranges::for_each(children, [&childrenDesiredSizes, space](const UIComponent* child) {
        if (const HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
        {
            const LONG line = slot->Line;
            if (childrenDesiredSizes.size() < static_cast<size_t>(line + 1L))
            {
                childrenDesiredSizes.resize(line + 1L, SIZE{});
            }
            else
            {
                childrenDesiredSizes[line].cx += space;
            }
            const SIZE childDesiredSize = child->DesiredSize;
            childrenDesiredSizes[line].cx += childDesiredSize.cx;
            childrenDesiredSizes[line].cy = std::max(childrenDesiredSizes[line].cy, childDesiredSize.cy);
        }
    });

    SIZE contentSize{};
    for (const auto& [lineWidth, lineHeight] : childrenDesiredSizes)
    {
        contentSize.cy += lineHeight;
    }

    const HorizontalAlignment horizontalAlign = HorizontalAlign;
    const VerticalAlignment   verticalAlign   = VerticalAlign;

    LONG defaultVerticalAlignPosition = AlignPoint()(verticalAlign, childrenAvailableSize.cy - contentSize.cy);

    std::vector<POINT>        alignPositionPerLine;
    alignPositionPerLine.resize(childrenDesiredSizes.size(), POINT{});
    LONG accumulatedHeight = 0;

    for (size_t line = 0; line < childrenDesiredSizes.size(); ++line)
    {
        const auto& [lineWidth, lineHeight]         = childrenDesiredSizes[line];
        alignPositionPerLine[line].x = AlignPoint()(horizontalAlign, childrenAvailableSize.cx - lineWidth);
        alignPositionPerLine[line].y = defaultVerticalAlignPosition + accumulatedHeight;
        accumulatedHeight += lineHeight;
    }

    std::vector<LONG> childrenDesiredWidthPerLine;
    childrenDesiredWidthPerLine.resize(childrenDesiredSizes.size(), 0);

    LONG lineSpace = LineSpace;

    std::ranges::for_each(children, [this, &childrenDesiredWidthPerLine, &alignPositionPerLine, space, lineSpace](UIComponent* child) {
        if (const HorizontalPanelSlot* slot = child->GetComponent<HorizontalPanelSlot>(); nullptr != slot)
        {
            const SIZE childAvailableSize = child->DesiredSize;

            const POINT absolutePosition = AbsoluteChildPosition;
            POINT       alignPosition    = alignPositionPerLine[slot->Line];
            alignPosition.x += childrenDesiredWidthPerLine[slot->Line];
            alignPosition.y += lineSpace * slot->Line;
            const POINT childPosition = absolutePosition + alignPosition;

            child->Arrange(childPosition, childAvailableSize);

            childrenDesiredWidthPerLine[slot->Line] += childAvailableSize.cx + space;
        }
    });

    return actualSize;
}

std::vector<HorizontalPanelSlot*> HorizontalPanel::GetSlots(const GameObject& parentGameObject)
{
    return parentGameObject.GetComponents<HorizontalPanelSlot>();
}

HorizontalPanelSlot::HorizontalPanelSlot() = default;

void HorizontalPanelSlot::ImGuiDrawPropertysEvent()
{
    SlotComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        const LONG line = ReflectFields->Line;
        ImGuiDebug()("Line", line);
    }
}

void HorizontalPanelSlot::SetLine(const LONG line)
{
    ReflectFields->Line = line;
}