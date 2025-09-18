#pragma once


class ScrollingWrapper : public UIComponent
{
    USING_PROPERTY(ScrollingWrapper)

public:
    ScrollingWrapper();

public:
    REFLECT_PROPERTY(Scroll)

    GETTER(float, Scroll) { return ReflectFields->Scroll; }
    SETTER(float, Scroll)
    {
        ReflectFields->Scroll = std::clamp(value, 0.0f, 1.0f);
        InvalidateArrange();
    }
    PROPERTY(Scroll)

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    float Scroll = 0.0f;
    REFLECT_FIELDS_END(ScrollingWrapper)
};