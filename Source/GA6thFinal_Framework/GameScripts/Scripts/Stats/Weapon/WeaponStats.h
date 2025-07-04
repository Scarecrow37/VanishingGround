#pragma once
#include <Stats/TurnActorStats.h>

struct WeaponStats : public TurnActorStats
{
    USING_PROPERTY(WeaponStats)

    enum class WeaponType
    {
        SWORD,    // 검
        DAGGER,   // 단검
        WARHAMMER // 대형 망치
    };
    
    REFLECT_PROPERTY(Name, Type, HitDamage, CriticalDamage, Speed, AttackCount)

    GETTER_ONLY(std::string_view, Name) { return ReflectFields->Name; }
    void SetName(std::string_view name) { ReflectFields->Name = name; }
    PROPERTY(Name) 

    GETTER(WeaponType, Type) { return ReflectFields->Type; }
    SETTER(WeaponType, Type) { ReflectFields->Type = value; }
    PROPERTY(Type)

    GETTER(int, HitDamage) { return ReflectFields->HitDamage; }
    SETTER(int, HitDamage) { ReflectFields->HitDamage = value; }
    PROPERTY(HitDamage)

    GETTER(int, CriticalDamage) { return ReflectFields->CriticalDamage; }
    SETTER(int, CriticalDamage) { ReflectFields->CriticalDamage = value; }
    PROPERTY(CriticalDamage)

    GETTER(int, Speed) { return ReflectFields->Speed; }
    SETTER(int, Speed) { ReflectFields->Speed = value; }
    PROPERTY(Speed)

    GETTER(int, AttackCount) { return ReflectFields->AttackCount; }
    SETTER(int, AttackCount) { ReflectFields->AttackCount = value; }
    PROPERTY(AttackCount)

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