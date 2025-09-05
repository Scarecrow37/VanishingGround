#include "pch.h"
#include "BoxSpacing.h"

BoxSpacing::BoxSpacing() = default;

BoxSpacing::BoxSpacing(const LONG left, const LONG top, const LONG right, const LONG bottom)
    : RECT{left, top, right, bottom}
{
}

BoxSpacing::BoxSpacing(const RECT& rect) : RECT{rect.left, rect.top, rect.right, rect.bottom} {}

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