#pragma once

class DummyElement : public UIComponent
{
    USING_PROPERTY(DummyElement)
public:
    DummyElement();

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(DummyElement)
};