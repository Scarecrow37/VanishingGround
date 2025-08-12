#include "pchScripts.h"
#include "RatioWrapper.h"

SIZE RatioWrapper::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    const float    ratio              = Ratio;

    SIZE appliedAvailableSize = availableSize;
    if (const LONG ratioHeight = static_cast<LONG>(static_cast<float>(availableSize.cx) * ratio);
        ratioHeight < availableSize.cy)
    {
        appliedAvailableSize.cy = ratioHeight;
        if (verticalFillMode == FillMode::FILL)
        {
            VerticalFillMode = FillMode::NONE;
            HorizontalFillMode = FillMode::FILL;
        }
    }
    else
    {
        const LONG ratioWidth   = static_cast<LONG>(static_cast<float>(availableSize.cy) / ratio);
        appliedAvailableSize.cx = ratioWidth;
        if (horizontalFillMode == FillMode::FILL)
        {
            HorizontalFillMode = FillMode::NONE;
            VerticalFillMode   = FillMode::FILL;
        }
    }

    const SIZE appliedRequestSize{.cx = _requestedSize.cx,
                                  .cy = static_cast<LONG>(static_cast<float>(_requestedSize.cx) * ratio)};


    SIZE desiredSize;
    if (horizontalFillMode == FillMode::FILL || verticalFillMode == FillMode::FILL ||
        appliedAvailableSize.cx < appliedRequestSize.cx)
    {
        desiredSize = appliedAvailableSize;
    }
    else
    {
        desiredSize = appliedRequestSize;
    }

    const PADDING padding            = Padding;
    const SIZE    childAvailableSize = desiredSize - padding.Size();
    SIZE          childDesiredSize{};
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        children[0]->Measure(childAvailableSize);
        childDesiredSize = children[0]->DesiredSize;
    }

    SIZE appliedWrapSize = childDesiredSize;
    if (const LONG ratioHeight = static_cast<LONG>(static_cast<float>(childDesiredSize.cx) * ratio);
        ratioHeight > childDesiredSize.cy)
    {
        appliedWrapSize.cy = ratioHeight;
        if (verticalFillMode == FillMode::WRAP)
        {
            VerticalFillMode = FillMode::NONE;
            HorizontalFillMode = FillMode::WRAP;
        }
    }
    else
    {
        const LONG ratioWidth   = static_cast<LONG>(static_cast<float>(appliedWrapSize.cy) / ratio);
        appliedWrapSize.cx    = ratioWidth;
        if (horizontalFillMode == FillMode::WRAP)
        {
            HorizontalFillMode = FillMode::NONE;
            VerticalFillMode   = FillMode::WRAP;
        }
    }
    appliedWrapSize += padding.Size();

    if (horizontalFillMode == FillMode::WRAP || verticalFillMode == FillMode::WRAP)
        desiredSize = appliedWrapSize;

    return desiredSize;
}

SIZE RatioWrapper::ArrangeOverride(const SIZE finalSize)
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