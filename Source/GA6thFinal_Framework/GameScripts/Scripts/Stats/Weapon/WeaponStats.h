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
    
    REFLECT_PROPERTY(Name, Type, HitDamage, CriticalDamageMultiplier, Speed, AttackCount, RandomSpeed)

    GETTER_ONLY(std::string_view, Name) { return ReflectFields->Name; }
    void SetName(std::string_view name) { ReflectFields->Name = name; }
    PROPERTY(Name) 

    GETTER(WeaponType, Type) { return ReflectFields->Type; }
    SETTER(WeaponType, Type) { ReflectFields->Type = value; }
    PROPERTY(Type)

    GETTER(int, HitDamage) { return ReflectFields->HitDamage; }
    SETTER(int, HitDamage) { ReflectFields->HitDamage = value; }
    PROPERTY(HitDamage)

    GETTER(float, CriticalDamageMultiplier) { return ReflectFields->CriticalDamageMultiplier; }
    SETTER(float, CriticalDamageMultiplier) { ReflectFields->CriticalDamageMultiplier = value; }
    //크리티컬 데미지 비율
    PROPERTY(CriticalDamageMultiplier)

    GETTER(int, Speed) { return ReflectFields->Speed; }
    SETTER(int, Speed) { ReflectFields->Speed = value; }
    PROPERTY(Speed)

    GETTER(int, AttackCount) { return ReflectFields->AttackCount; }
    SETTER(int, AttackCount) { ReflectFields->AttackCount = value; }
    //연격 횟수
    PROPERTY(AttackCount)

    GETTER(int, AttackPerChain) { return ReflectFields->AttackPerChain; }
    SETTER(int, AttackPerChain) { ReflectFields->AttackPerChain = value; }
    //공격 당 연격 부여량
    PROPERTY(AttackPerChain)

    /*랜덤 속도를 갱신합니다.*/
    int RollRandomSpeed();
    GETTER_ONLY(int, RandomSpeed) { return _randomSpeed; }
    PROPERTY(RandomSpeed)
    
    WeaponStats() = default;
    virtual ~WeaponStats() override = default;

protected:
    REFLECT_FIELDS_BEGIN(TurnActorStats)
    std::string Name = "Default Sword";
    WeaponType  Type = WeaponType::SWORD;
    int         HitDamage = 1;
    float       CriticalDamageMultiplier = 1.5f;
    int         Speed = 0;
    int         AttackCount = 1;
    int         AttackPerChain = 1;
    REFLECT_FIELDS_END(WeaponStats)
    int _randomSpeed = 0;

public:
    WeaponStats(const WeaponStats& rhs) 
    {     
        auto& myRf = *ReflectFields;
        auto& rhsRf = *rhs.ReflectFields;
        myRf = rhsRf;
    }
    WeaponStats& operator=(const WeaponStats& rhs)
    {
        if (this != &rhs)
        {
            auto& myRf  = *ReflectFields;
            auto& rhsRf = *rhs.ReflectFields;
            myRf        = rhsRf;
        }
        return *this;
    }

};