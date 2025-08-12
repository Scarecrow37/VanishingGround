#include "pchScripts.h"
#include "DummyElement.h"

DummyElement::DummyElement() = default;

SIZE DummyElement::MeasureOverride(const SIZE availableSize)
{
    const SIZE desiredSize = MinSize()(availableSize, _requestedSize);
    return desiredSize;
}

SIZE DummyElement::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE actualSize  = MinSize()(finalSize, desiredSize);
    return actualSize;
}