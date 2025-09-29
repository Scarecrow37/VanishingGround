#pragma once

class SpawnDamageElement : public UIComponent
{
    USING_PROPERTY(SpawnDamageElement)

public:
    REFLECT_PROPERTY(Radius)

    GETTER(float, Radius) { return ReflectFields->Radius; }
    SETTER(float, Radius) { ReflectFields->Radius = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(Radius)

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void OnDrawDebugSelectedOverride() override;

    void Start() override;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    float Radius = 1.0f;
    REFLECT_FIELDS_END(SpawnDamageElement)

    UIComponent* _child;
};
