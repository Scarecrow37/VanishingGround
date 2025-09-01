#pragma once

class ParticleComponent;

class EffectManager : public Component
{
    USING_PROPERTY(EffectManager)
    inline static EffectManager* _staticInstance;

public:
    EffectManager();
    ~EffectManager();
    inline static EffectManager* GetInstance() { return _staticInstance; }

private:
    void Awake() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    bool SpawnWeaponHitEffect(const Vector3& position, 
                              const Vector3& rotation = Vector3(0.0f, 0.0f, 0.0f),
                              const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f));

private:
    // Hit이펙트 풀링
    std::vector<ParticleComponent*> _weaponHitEffectPool;   // 이펙트 풀
    int _currentWeaponHitEffectIndex = 0; // 현재 사용할 이펙트 인덱스

    REFLECT_FIELDS_BEGIN(Component)
    std::string HitEffectVFXGuid;
    int WeaponHitEffectPoolSize = 10; // 이펙트 풀 크기
    REFLECT_FIELDS_END(EffectManager)
};
