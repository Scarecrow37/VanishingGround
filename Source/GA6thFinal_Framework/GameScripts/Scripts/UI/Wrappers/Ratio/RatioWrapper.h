#pragma once


class RatioWrapper : public UIComponent
{
    USING_PROPERTY(RatioWrapper)

public:
    RatioWrapper();

public:
    REFLECT_PROPERTY(Ratio)

    GETTER(float, Ratio) { return ReflectFields->Ratio; }
    SETTER(float, Ratio)
    {
        ReflectFields->Ratio = std::clamp(value, 0.01f, 100.0f);
        InvalidateMeasure();
    }
    PROPERTY(Ratio)

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    float Ratio = 1.0f;
    REFLECT_FIELDS_END(RatioWrapper)
};