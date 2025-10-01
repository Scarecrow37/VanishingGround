#pragma once

class DamageElement;

class SpawnDamagePanel : public UIComponent, public InputReceiver
{
    USING_PROPERTY(SpawnDamagePanel)

public:
    SpawnDamagePanel();

public:
    REFLECT_PROPERTY(RadiusRatio, LifeTime)

    GETTER(float, RadiusRatio) { return ReflectFields->Radius; }
    SETTER(float, RadiusRatio) { ReflectFields->Radius = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(RadiusRatio)

    GETTER(float, LifeTime)
    {
        return ReflectFields->LifeTime;
    }
    SETTER(float, LifeTime)
    {
        ReflectFields->LifeTime = std::max(0.1f, value);
    }
    PROPERTY(LifeTime)

    GETTER_ONLY(float, Radius)
    {
        const SIZE       size   = Size;
        const LONG       min  = std::min(size.cx, size.cy);
        return static_cast<float>(min) * 0.5f;
    }
    PROPERTY(Radius)


protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void OnDrawDebugSelectedOverride() override;

    void Awake() override;
    void Reset() override;

    void                    EraseChild() const;
    DamageElement*          MakeDamage() const;
    std::pair<POINT, float> GetRandomSpawnPointAndAngle() const;

    void OnButton(const Input::Controller& controller);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    float Radius = 1.0f;
    float LifeTime = 1.0f;
    REFLECT_FIELDS_END(SpawnDamagePanel)

    std::vector<DamageElement*> _damageElements;
};
