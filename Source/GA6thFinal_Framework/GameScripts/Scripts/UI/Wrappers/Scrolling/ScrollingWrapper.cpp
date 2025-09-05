#include "pchScripts.h"
#include "ScrollingWrapper.h"

ScrollingWrapper::ScrollingWrapper() = default;

SIZE ScrollingWrapper::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    SIZE           desiredSize        = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                 verticalFillMode == FillMode::FILL);

    const PADDING padding            = Padding;
    SIZE    childAvailableSize = desiredSize - padding.Size();
    childAvailableSize.cy            = LONG_MAX;
    SIZE          childDesiredSize{};
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        children[0]->Measure(childAvailableSize);
        childDesiredSize = children[0]->DesiredSize;
    }

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = childDesiredSize.cx + padding.Horizontal();

    return desiredSize;
}

SIZE ScrollingWrapper::ArrangeOverride(const SIZE finalSize)
{
    const SIZE    desiredSize        = DesiredSize;
    const SIZE    actualSize         = MinSize()(finalSize, desiredSize);
    const PADDING padding            = Padding;
    SIZE    childAvailableSize = actualSize - padding.Size();
    childAvailableSize.cy            = LONG_MAX;
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        const HorizontalAlignment horizontalAlign = HorizontalAlign;
        const SIZE                childSize       = children[0]->DesiredSize;
        POINT                     childPoint      = AbsoluteChildPosition;
        const LONG                alignX          = AlignPoint()(horizontalAlign, (childAvailableSize - childSize).cx);
        childPoint.x += alignX;
        const float scroll = Scroll;
        constexpr LONG  minScrollOffset = 0;
        const LONG  maxScrollOffset = std::max(0L, childSize.cy - actualSize.cy);
        const LONG      scrollOffset    = static_cast<LONG>(std::lerp(minScrollOffset, maxScrollOffset, scroll));
        childPoint.y -= scrollOffset;

        children[0]->Arrange(childPoint, childAvailableSize);
    }

    return actualSize;
}
