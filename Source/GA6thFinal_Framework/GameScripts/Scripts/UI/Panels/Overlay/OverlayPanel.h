#pragma once

class OverlayPanel : public UIComponent
{
    USING_PROPERTY(OverlayPanel)

public:
    OverlayPanel();

public:
    REFLECT_PROPERTY()

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(OverlayPanel)
};