#include "pchScripts.h"
#include "DummyWrapper.h"

DummyWrapper::DummyWrapper() = default;

SIZE DummyWrapper::MeasureOverride(const SIZE availableSize)
{
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        children[0]->Measure(availableSize);
    }

    const SIZE size = {.cx = std::min(availableSize.cx, _requestedSize.cx),
                       .cy = std::min(availableSize.cy, _requestedSize.cy)};
    return size;
}

SIZE DummyWrapper::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE size = {.cx = std::min(finalSize.cx, desiredSize.cx), .cy = std::min(finalSize.cy, desiredSize.cy)};

    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        const POINT absolutePosition = AbsolutePosition;
        children[0]->Arrange(absolutePosition, size);
    }

    return size;
}