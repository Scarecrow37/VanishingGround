#pragma once

class GameObject;
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
    /// <summary>
    /// 무기 피격 파티클을 지정된 게임 오브젝트에 생성합니다.
    /// </summary>
    /// <param name="dest">파티클을 생성할 대상 GameObject의 포인터입니다.</param>
    /// <param name="offsetPosition">파티클의 위치 오프셋입니다. 기본값은 (0.0f, 0.0f, 0.0f)입니다.</param>
    /// <param name="offsetRotation">파티클의 회전 오프셋입니다. 기본값은 (0.0f, 0.0f, 0.0f)입니다.</param>
    /// <param name="offsetScale">파티클의 크기 오프셋입니다. 기본값은 (0.0f, 0.0f, 0.0f)입니다.</param>
    /// <returns>파티클 생성에 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool SpawnWeaponHitParticle(GameObject* dest,
                                const Vector3& offsetPosition = Vector3(0.0f, 0.0f, 0.0f), 
                                const Vector3& offsetRotation = Vector3(0.0f, 0.0f, 0.0f),
                                const Vector3& offsetScale    = Vector3(0.0f, 0.0f, 0.0f));

private:
    // Hit이펙트 풀링
    std::vector<ParticleComponent*> _weaponHitEffectPool;   // 무기 Hit 이펙트 풀
    int _currentWeaponHitEffectIndex = 0;                   // 다음에 사용할 무기 Hit 이펙트 인덱스

    REFLECT_FIELDS_BEGIN(Component)
    std::string WeaponHitEffectVFXGuid;     // 무기 Hit 이펙트 VFX 파일 GUID
    int WeaponHitEffectPoolSize = 10;       // 무기 Hit 이펙트 풀 크기
    REFLECT_FIELDS_END(EffectManager)
};
