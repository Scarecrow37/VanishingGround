#pragma once
#include <Stats/TurnActorStats.h>

struct WeaponStats : public TurnActorStats
{
    USING_PROPERTY(WeaponStats)
    
    REFLECT_PROPERTY(ReflectFields->Name, ReflectFields->Type)

    enum class WeaponType
    {
        SWORD,    // 검
        DAGGER,   // 단검
        WARHAMMER // 대형 망치
    };

    WeaponStats() = default;
    virtual ~WeaponStats() override = default;

protected:
    REFLECT_FIELDS_BEGIN(TurnActorStats)
    std::string Name;
    WeaponType  Type = WeaponType::SWORD;
    int         HitDamage = 1;
    int         CriticalDamage = 2;
    int         Speed = 0;
    int         AttackCount = 1;
    REFLECT_FIELDS_END(WeaponStats)

};