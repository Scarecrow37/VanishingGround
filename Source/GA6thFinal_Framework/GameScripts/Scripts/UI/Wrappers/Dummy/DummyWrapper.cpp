#include "pchScripts.h"
#include "DummyWrapper.h"

DummyWrapper::DummyWrapper() = default;

SIZE DummyWrapper::MeasureOverride(const SIZE availableSize)
{
    const SIZE desiredSize = MinSize()(availableSize, _requestedSize);
    const SIZE childAvailableSize = desiredSize - _padding.Size();
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        children[0]->Measure(childAvailableSize);
    }

    return desiredSize;
}

SIZE DummyWrapper::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE actualSize = MinSize()(finalSize, desiredSize);
    const SIZE childAvailableSize = actualSize - _padding.Size();
    if (const std::vector<UIComponent*> children = Children; children.size() > 0)
    {
        const POINT childPoint = AbsoluteChildPosition;
        children[0]->Arrange(childPoint, childAvailableSize);
    }

    return actualSize;
}