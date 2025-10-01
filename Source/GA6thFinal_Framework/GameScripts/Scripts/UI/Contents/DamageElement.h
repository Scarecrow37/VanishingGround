#pragma once

class DamageElement : public UIComponent
{
    USING_PROPERTY(DamageElement)

public:
    DamageElement();

    void Setup(LONG distance, float angle, float duration, POINT origin);

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void Update() override;

    POINT GetPoint() const;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    REFLECT_FIELDS_END(DamageElement)

    POINT _origin;
    LONG _distance;
    float _angle;
    float _duration;
    float _elapsedTime;
    std::vector<float> _steps;
    std::vector<Vector3> _points;
};
