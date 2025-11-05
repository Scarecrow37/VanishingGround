#pragma once

class IFontAppearance
{
public:
    IFontAppearance()                        = default;
    IFontAppearance(const IFontAppearance&)      = default;
    IFontAppearance& operator=(const IFontAppearance&) = default;
    IFontAppearance(IFontAppearance&&)                 = default;
    IFontAppearance& operator=(IFontAppearance&&)      = default;
    virtual ~IFontAppearance()                     = default;

    virtual void SetFontWeight(float fontWeight) = 0;
};