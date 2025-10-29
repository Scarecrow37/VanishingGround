#include "pchScripts.h"
#include "OverlayPanel.h"


UMREAL_COMPONENT(OverlayPanel)

OverlayPanel::OverlayPanel() = default;

SIZE OverlayPanel::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    SIZE           desiredSize        = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                 verticalFillMode == FillMode::FILL);

    const PADDING padding            = Padding;
    const SIZE    childAvailableSize = desiredSize - padding.Size();
    SIZE          childrenDesiredSize{};
    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [childAvailableSize, &childrenDesiredSize](UIComponent* child) {
        child->Measure(childAvailableSize);
        const SIZE childDesiredSize = child->DesiredSize;
        childrenDesiredSize         = MaxSize()(childrenDesiredSize, childDesiredSize);
    });

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = childrenDesiredSize.cx + padding.Horizontal();
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = childrenDesiredSize.cy + padding.Vertical();

    return desiredSize;
}

SIZE OverlayPanel::ArrangeOverride(const SIZE finalSize)
{
    const SIZE    desiredSize        = DesiredSize;
    const SIZE    actualSize         = MinSize()(finalSize, desiredSize);
    const PADDING padding            = Padding;
    const SIZE    childAvailableSize = actualSize - padding.Size();
    const HorizontalAlignment       horizontalAlign    = HorizontalAlign;
    const VerticalAlignment         verticalAlign      = VerticalAlign;
    const POINT                     absoluteChildPosition = AbsoluteChildPosition;

    const std::vector<UIComponent*> children = Children;
    std::ranges::for_each(children, [childAvailableSize, horizontalAlign, verticalAlign,
                                     absoluteChildPosition](UIComponent* child) {
        const SIZE  childDesiredSize = child->DesiredSize;
        const POINT alignPosition = AlignPoint()(horizontalAlign, verticalAlign, childAvailableSize - childDesiredSize);
        const POINT childPoint    = absoluteChildPosition + alignPosition;
        child->Arrange(childPoint, childAvailableSize);
    });

    return actualSize;
}