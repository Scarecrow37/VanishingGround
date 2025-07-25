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
    
    REFLECT_PROPERTY(
        Name, 
        Type, 
        HitDamage, 
        HitDamageMultiplier, 
        CriticalDamage,
        CriticalDamageMultiplier, 
        Speed, 
        AttackCount, 
        AttackPerChain,
        AttackPerChainMultiplier,
        RandomSpeed)

    GETTER_ONLY(const std::string&, Name) { return ReflectFields->Name; }
    void SetName(const std::string& name) { ReflectFields->Name = name; }
    //string_view 무기 이름
    PROPERTY(Name) 

    GETTER(WeaponType, Type) { return ReflectFields->Type; }
    SETTER(WeaponType, Type) { ReflectFields->Type = value; }
    //WeaponType 무기 타입. 
    PROPERTY(Type)

    GETTER(int, HitDamage) { return ReflectFields->HitDamage; }
    SETTER(int, HitDamage) { ReflectFields->HitDamage = value; }
    //int 일반 데미지
    PROPERTY(HitDamage)

    SETTER(float, HitDamageMultiplier) { ReflectFields->HitDamageMultiplier = std::max(value, 0.f); }
    GETTER(float, HitDamageMultiplier) { return ReflectFields->HitDamageMultiplier; }
    //float 일반 데미지 배율
    PROPERTY(HitDamageMultiplier)

    SETTER(int, CriticalDamage) { ReflectFields->CriticalDamage = std::max(value, 1); }
    GETTER(int, CriticalDamage) { return ReflectFields->CriticalDamage; }
    //int 크리티컬 데미지
    PROPERTY(CriticalDamage)

    SETTER(float, CriticalDamageMultiplier) { ReflectFields->CriticalDamageMultiplier = std::max(value, 0.f); }
    GETTER(float, CriticalDamageMultiplier) { return ReflectFields->CriticalDamageMultiplier; }
    //float 크리티컬 데미지 배율
    PROPERTY(CriticalDamageMultiplier)

    SETTER(int, Speed) { ReflectFields->Speed = value; }
    GETTER(int, Speed) { return ReflectFields->Speed; }
    //int 무기 속도
    PROPERTY(Speed)

    SETTER(int, AttackCount) { ReflectFields->AttackCount = std::max(value, 1); }
    GETTER(int, AttackCount) { return ReflectFields->AttackCount; }
    //int 연격 횟수
    PROPERTY(AttackCount)

    SETTER(int, AttackPerChain) { ReflectFields->AttackPerChain = std::clamp(value, 0, 99); }
    GETTER(int, AttackPerChain) { return ReflectFields->AttackPerChain; }
    //int 공격 당 연격 부여량
    PROPERTY(AttackPerChain)

    SETTER(float, AttackPerChainMultiplier) { ReflectFields->AttackPerChainMultiplier = std::max(value, 0.f); }
    GETTER(float, AttackPerChainMultiplier) { return ReflectFields->AttackPerChainMultiplier; }
    //float 공격 당 연격 부여량 배율
    PROPERTY(AttackPerChainMultiplier)

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
    float       HitDamageMultiplier = 1.f;
    int         CriticalDamage = 2;
    float       CriticalDamageMultiplier = 1.f;
    int         Speed = 0;
    int         AttackCount = 1;
    int         AttackPerChain = 1;
    float       AttackPerChainMultiplier = 1.f;
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