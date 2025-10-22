#pragma once

class DamageElement;

class SpawnDamagePanel : public UIComponent, public InputReceiver
{
    USING_PROPERTY(SpawnDamagePanel)

public:
    SpawnDamagePanel();

public:
    REFLECT_PROPERTY(RadiusRatio, LifeTime, FilePath, Color, FontScale)

    GETTER_ONLY(std::string, FilePath) { return _Guid.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(DirectX::SimpleMath::Color, Color) { return DirectX::SimpleMath::Color(&ReflectFields->Color[0]); }
    SETTER(DirectX::SimpleMath::Color, Color)
    {
        std::memcpy(&ReflectFields->Color[0], &value.x, sizeof(ReflectFields->Color));
    }
    PROPERTY(Color)

    GETTER(float, FontScale) { return ReflectFields->FontScale; }
    SETTER(float, FontScale) { ReflectFields->FontScale = std::max(1.0f, value); }
    PROPERTY(FontScale)

    GETTER(float, RadiusRatio) { return ReflectFields->Radius; }
    SETTER(float, RadiusRatio) { ReflectFields->Radius = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(RadiusRatio)

    GETTER(float, LifeTime) { return ReflectFields->LifeTime; }
    SETTER(float, LifeTime) { ReflectFields->LifeTime = std::max(0.1f, value); }
    PROPERTY(LifeTime)

    GETTER_ONLY(float, Radius)
    {
        const SIZE size = Size;
        const LONG min  = std::min(size.cx, size.cy);
        return static_cast<float>(min) * 0.5f;
    }
    PROPERTY(Radius)

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void OnDrawDebugSelectedOverride() override;
    void DeserializedReflectEvent() override;

    void Awake() override;
    void Reset() override;

    void                    EraseChild() const;
    DamageElement*          MakeDamage() const;
    std::pair<POINT, float> GetRandomSpawnPointAndAngle() const;

    void OnButton(const Input::Controller& controller);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    float                Radius   = 1.0f;
    float                LifeTime = 1.0f;
    std::string          Guid;
    std::array<float, 4> Color     = {-.0f, 0.0f, 0.0f, 1.0f};
    float                FontScale = 32.0f;
    REFLECT_FIELDS_END(SpawnDamagePanel)

private:
    File::Guid _Guid;
    std::vector<DamageElement*> _damageElements;
};
