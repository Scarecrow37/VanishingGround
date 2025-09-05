#pragma once

class DummyWrapper : public UIComponent
{
    USING_PROPERTY(DummyWrapper)
public:
    DummyWrapper();

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(DummyWrapper)
};