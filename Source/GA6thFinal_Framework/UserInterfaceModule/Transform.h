#pragma once

namespace UI::Transform
{
    struct Point
    {
        long X;
        long Y;
    };

    struct Size
    {
        long Width;
        long Height;
    };

    struct Rect
    {
        Point Point;
        Size  Size;
    };

    struct Padding
    {
        long Left;
        long Right;
        long Top;
        long Bottom;
    };

    struct Margin
    {
        long Left;
        long Right;
        long Top;
        long Bottom;
    };
} // namespace UI::Transform