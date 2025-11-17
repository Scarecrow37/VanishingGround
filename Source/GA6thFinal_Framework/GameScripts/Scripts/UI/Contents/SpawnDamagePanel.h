#pragma once

class DamageElement;

class SpawnDamagePanel : public UIComponent
{
    USING_PROPERTY(SpawnDamagePanel)

public:
    SpawnDamagePanel();

public:
    REFLECT_PROPERTY(RadiusRatio, LifeTime, FilePath, BeginColor, EndColor, BeginOutlineColor, EndOutlineColor,
                     BeginScale, EndScale, TurningPoint, EasingFunctionType)

    GETTER_ONLY(std::string, FilePath) { return _Guid.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(DirectX::SimpleMath::Color, BeginColor) { return Color(&ReflectFields->BeginColor[0]); }
    SETTER(DirectX::SimpleMath::Color, BeginColor)
    {
        std::memcpy(&ReflectFields->BeginColor[0], &value.x, sizeof(ReflectFields->BeginColor));
    }
    PROPERTY(BeginColor)

    GETTER(DirectX::SimpleMath::Color, EndColor) { return Color(&ReflectFields->EndColor[0]); }
    SETTER(DirectX::SimpleMath::Color, EndColor)
    {
        std::memcpy(&ReflectFields->EndColor[0], &value.x, sizeof(ReflectFields->EndColor));
    }
    PROPERTY(EndColor)

    GETTER(DirectX::SimpleMath::Color, BeginOutlineColor) { return Color(&ReflectFields->BeginOutlineColor[0]); }
    SETTER(DirectX::SimpleMath::Color, BeginOutlineColor)
    {
        std::memcpy(&ReflectFields->BeginOutlineColor[0], &value.x, sizeof(ReflectFields->BeginOutlineColor));
    }
    PROPERTY(BeginOutlineColor)

    GETTER(DirectX::SimpleMath::Color, EndOutlineColor) { return Color(&ReflectFields->EndOutlineColor[0]); }
    SETTER(DirectX::SimpleMath::Color, EndOutlineColor)
    {
        std::memcpy(&ReflectFields->EndOutlineColor[0], &value.x, sizeof(ReflectFields->EndOutlineColor));
    }
    PROPERTY(EndOutlineColor)

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

    GETTER(float, TurningPoint) { return ReflectFields->TurningPoint; }
    SETTER(float, TurningPoint) { ReflectFields->TurningPoint = std::clamp(value, 0.1f, 0.9f); }
    PROPERTY(TurningPoint)

    GETTER_ONLY(float, Radius)
    {
        const SIZE size = Size;
        const LONG min  = std::min(size.cx, size.cy);
        return static_cast<float>(min) * 0.5f;
    }
    PROPERTY(Radius)

    GETTER(Mathf::EasingFunctionType, EasingFunctionType) { return ReflectFields->EasingFunctionType; }
    SETTER(Mathf::EasingFunctionType, EasingFunctionType) { ReflectFields->EasingFunctionType = value; }
    PROPERTY(EasingFunctionType)

public:
    std::weak_ptr<DamageElement> MakeDamage(int                    damage,
                                            std::span<const std::string> revelations = std::span<const std::string>()) const;

protected:
    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void OnDrawDebugSelectedOverride() override;
    void DeserializedReflectEvent() override;

    void Reset() override;

    void                    EraseChild() const;
    std::pair<POINT, float> GetRandomSpawnPointAndAngle() const;

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    float                     Radius   = 1.0f;
    float                     LifeTime = 1.0f;
    std::string               Guid;
    std::array<float, 4>      BeginColor         = {0.0f, 0.0f, 0.0f, 0.5f};
    std::array<float, 4>      EndColor           = {0.0f, 0.0f, 0.0f, 1.0f};
    std::array<float, 4>      BeginOutlineColor  = {1.0f, 1.0f, 1.0f, 0.5f};
    std::array<float, 4>      EndOutlineColor    = {1.0f, 1.0f, 1.0f, 1.0f};
    float                     BeginScale         = 64.0f;
    float                     EndScale           = 48.0f;
    float                     TurningPoint       = 0.5f;
    Mathf::EasingFunctionType EasingFunctionType = Mathf::EasingFunctionType::QUAD;
    REFLECT_FIELDS_END(SpawnDamagePanel)

private:
    File::Guid _Guid;
};
