#pragma once

class DamageElement;

class SpawnDamagePanel : public UIComponent, public InputReceiver
{
    USING_PROPERTY(SpawnDamagePanel)

public:
    SpawnDamagePanel();

public:
    REFLECT_PROPERTY(RadiusRatio, LifeTime, FilePath, BeginColor, EndColor, BeginScale, EndScale)

    GETTER_ONLY(std::string, FilePath) { return _Guid.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(DirectX::SimpleMath::Color, BeginColor) { return DirectX::SimpleMath::Color(&ReflectFields->BeginColor[0]); }
    SETTER(DirectX::SimpleMath::Color, BeginColor)
    {
        std::memcpy(&ReflectFields->BeginColor[0], &value.x, sizeof(ReflectFields->BeginColor));
    }
    PROPERTY(BeginColor)

    GETTER(DirectX::SimpleMath::Color, EndColor) { return DirectX::SimpleMath::Color(&ReflectFields->EndColor[0]); }
    SETTER(DirectX::SimpleMath::Color, EndColor)
    {
        std::memcpy(&ReflectFields->EndColor[0], &value.x, sizeof(ReflectFields->EndColor));
    }
    PROPERTY(EndColor)

    GETTER(float, BeginScale) { return ReflectFields->BeginScale; }
    SETTER(float, BeginScale) { ReflectFields->BeginScale = std::max(1.0f, value); }
    PROPERTY(BeginScale)

    GETTER(float, EndScale) { return ReflectFields->EndScale; }
    SETTER(float, EndScale) { ReflectFields->EndScale = std::max(1.0f, value); }
    PROPERTY(EndScale)

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

    void                         EraseChild() const;
    std::weak_ptr<DamageElement> MakeDamage() const;
    std::pair<POINT, float>      GetRandomSpawnPointAndAngle() const;

    void OnButton(const Input::Controller& controller);

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    float                Radius   = 1.0f;
    float                LifeTime = 1.0f;
    std::string          Guid;
    std::array<float, 4> BeginColor = {0.0f, 0.0f, 0.0f, 0.5f};
    std::array<float, 4> EndColor   = {0.0f, 0.0f, 0.0f, 1.0f};
    float                BeginScale = 64.0f;
    float                EndScale   = 48.0f;
    REFLECT_FIELDS_END(SpawnDamagePanel)

private:
    File::Guid _Guid;
    std::vector<std::weak_ptr<DamageElement>> _damageElements;
};
