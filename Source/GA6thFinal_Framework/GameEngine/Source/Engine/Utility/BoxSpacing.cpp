#include "pch.h"
#include "BoxSpacing.h"

LONG BoxSpacing::Horizontal() const
{
    return left + right;
}

LONG BoxSpacing::Vertical() const
{
    return top + bottom;
}

POINT BoxSpacing::LeftTop() const
{
    return POINT{left, top};
}

SIZE BoxSpacing::Size() const
{
    return SIZE{Horizontal(), Vertical()};
}