#include "pchScripts.h"
#include "DummyWrapper.h"

DummyWrapper::DummyWrapper() = default;

SIZE DummyWrapper::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    SIZE           desiredSize        = MinSize()(availableSize, _requestedSize, verticalFillMode == FillMode::FILL,
                                 horizontalFillMode == FillMode::FILL);

    const PADDING padding            = Padding;
    const SIZE    childAvailableSize = desiredSize - padding.Size();
    SIZE          childDesiredSize{};
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        children[0]->Measure(childAvailableSize);
        childDesiredSize = children[0]->DesiredSize;
    }

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = childDesiredSize.cx + padding.Horizontal();
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = childDesiredSize.cy + padding.Vertical();

    return desiredSize;
}

SIZE DummyWrapper::ArrangeOverride(const SIZE finalSize)
{
    const SIZE    desiredSize        = DesiredSize;
    const SIZE    actualSize         = MinSize()(finalSize, desiredSize);
    const PADDING padding            = Padding;
    const SIZE    childAvailableSize = actualSize - padding.Size();
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        const HorizontalAlignment horizontalAlign = HorizontalAlign;
        const VerticalAlignment   verticalAlign   = VerticalAlign;
        const SIZE                childSize       = children[0]->DesiredSize;
        POINT                     childPoint      = AbsoluteChildPosition;
        childPoint += AlignPoint()(horizontalAlign, verticalAlign, childAvailableSize - childSize);

        children[0]->Arrange(childPoint, childAvailableSize);
    }

    return actualSize;
}