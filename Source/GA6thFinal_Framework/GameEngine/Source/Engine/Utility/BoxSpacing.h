#pragma once

typedef struct BoxSpacing : RECT
{
    BoxSpacing();
    BoxSpacing(LONG left, LONG top, LONG right, LONG bottom);
    explicit BoxSpacing(const RECT& rect);

    explicit operator RECT() const { return RECT{left, top, right, bottom}; }

    [[nodiscard]] LONG  Horizontal() const;
    [[nodiscard]] LONG  Vertical() const;
    [[nodiscard]] POINT LeftTop() const;
    [[nodiscard]] SIZE  Size() const;
} PADDING, MARGIN;