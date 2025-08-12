#pragma once

typedef struct BoxSpacing : RECT
{
    [[nodiscard]] LONG  Horizontal() const;
    [[nodiscard]] LONG  Vertical() const;
    [[nodiscard]] POINT LeftTop() const;
    [[nodiscard]] SIZE  Size() const;
} PADDING, MARGIN;