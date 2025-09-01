#include "pchScripts.h"
#include "EffectManager.h"
#include "Particle/ParticleComponent.h"

EffectManager::EffectManager() 
{
}

EffectManager::~EffectManager() 
{
}

void EffectManager::Awake() 
{
    // 기존 이펙트 풀 리셋
    for (int i = 0; i < _weaponHitEffectPool.size(); ++i)
    {
        gameObject->Destroy(_weaponHitEffectPool[i]);
    }
    _weaponHitEffectPool.clear();

    // HitEffectVFXGuid가 유효하면 이펙트 풀링 생성
    if (File::NULL_GUID != ReflectFields->HitEffectVFXGuid)
    {
        for (int i = 0; i < ReflectFields->WeaponHitEffectPoolSize; ++i)
        {
            ParticleComponent& effect = AddComponent<ParticleComponent>();
            _weaponHitEffectPool.push_back(&effect);
            // TODO: ParticleComponent에 vfx파일 로드하는 함수 만들기
        }
    }
}

void EffectManager::SerializedReflectEvent() 
{
}

void EffectManager::DeserializedReflectEvent() 
{
}

void EffectManager::ImGuiDrawPropertysEvent() 
{
}

bool EffectManager::SpawnWeaponHitEffect(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
    if (_weaponHitEffectPool.empty())
    {
        return false;
    }

    ParticleComponent* effect = _weaponHitEffectPool[_currentWeaponHitEffectIndex];
    if (effect)
    {
        effect->StopEffect();
        effect->PlayEffect();
        effect->Position = position;
        effect->Rotation = rotation;
        effect->Scale    = scale;
        // 다음 이펙트 인덱스 갱신
        _currentWeaponHitEffectIndex = (_currentWeaponHitEffectIndex + 1) % _weaponHitEffectPool.size();
        return true;
    }
