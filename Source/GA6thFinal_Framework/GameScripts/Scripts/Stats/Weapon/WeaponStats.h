#pragma once
#include <Stats/TurnActorStats.h>
#include <ColorUtils/ColorUtils.h>

enum class WeaponType
{
    SWORD,    // 검
    DAGGER,   // 단검
    WARHAMMER // 대형 망치
};

enum class WeaponGrade
{
    COMMON,     //일반
    RARE,       //희귀
    BIZARRE,    //기이
};

struct WeaponStats : public TurnActorStats
{
    inline static constexpr const char* DEFAULT_NAME = "Default Sword";

    inline static constexpr ImVec4 GetTypeToColor(WeaponType type)
    {
        switch (type)
        {
        case WeaponType::SWORD:
            return {1.f, 0.f, 0.f, 1.f}; // 빨강
        case WeaponType::DAGGER:
            return {0.44f, 0.68f, 0.44f, 1.f}; // 초록
        case WeaponType::WARHAMMER:
            return {1.f, 0.85f, 0.4f, 1.f}; // 노랑
        default:
            return {0.5f, 0.5f, 0.5f, 1.f}; // 기본 회색
        }
    }

    inline static constexpr ImVec4 GetGradeToColor(WeaponGrade grade)
    {
        switch (grade)
        {
        case WeaponGrade::COMMON:
            return {1.f, 1.f, 1.f, 1.f}; // 흰색
        case WeaponGrade::RARE:
            return {0.227f, 0.718f, 1.f, 1.f}; // 파랑
        case WeaponGrade::BIZARRE:
            return {1.f, 0.f, 1.f, 1.f}; // 보라
        default:
            return {0.5f, 0.5f, 0.5f, 1.f}; // 기본 회색
        }
    }

    inline static constexpr int GetGradeID(WeaponGrade grade)
    {
        switch (grade)
        {
        case WeaponGrade::COMMON:
            return 300000;
        case WeaponGrade::RARE:
            return 300001;
        case WeaponGrade::BIZARRE:
            return 300002;
        default:
            return -1;
        }
    }

    USING_PROPERTY(WeaponStats)
    
    REFLECT_PROPERTY(
        WeaponID, 
        WeaponName, 
        Type, 
        Grade,
        HitDamage, 
        HitDamageMultiplier, 
        CriticalDamage,
        CriticalDamageMultiplier, 
        Speed, 
        AttackCount, 
        AttackPerChain,
        AttackPerChainMultiplier,
        RandomSpeed)

    SETTER(int, WeaponID) { ReflectFields->WeaponID = std::max(value, 0); }
    GETTER(int, WeaponID) { return ReflectFields->WeaponID; }
    PROPERTY(WeaponID)

    GETTER_ONLY(const std::string&, WeaponName) { return ReflectFields->Name; }
    void SetName(const std::string& weaponName) { ReflectFields->Name = weaponName; }
    //string_view 무기 이름
    PROPERTY(WeaponName) 

    GETTER(WeaponType, Type) { return ReflectFields->Type; }
    SETTER(WeaponType, Type) { ReflectFields->Type = value; }
    //WeaponType 무기 타입. 
    PROPERTY(Type)

    SETTER(WeaponGrade, Grade) { ReflectFields->Grade = value; }
    GETTER(WeaponGrade, Grade) { return ReflectFields->Grade; }
    //무기 등급
    PROPERTY(Grade)

    GETTER(int, HitDamage) { return ReflectFields->HitDamage; }
    SETTER(int, HitDamage) { ReflectFields->HitDamage = value; }
    //int 일반 데미지
    PROPERTY(HitDamage)

    SETTER(float, HitDamageMultiplier) { ReflectFields->HitDamageMultiplier = value; }
    GETTER(float, HitDamageMultiplier) { return ReflectFields->HitDamageMultiplier; }
    //float 일반 데미지 배율
    PROPERTY(HitDamageMultiplier)

    SETTER(int, CriticalDamage) { ReflectFields->CriticalDamage = value; }
    GETTER(int, CriticalDamage) { return ReflectFields->CriticalDamage; }
    //int 크리티컬 데미지
    PROPERTY(CriticalDamage)

    SETTER(float, CriticalDamageMultiplier) { ReflectFields->CriticalDamageMultiplier = value; }
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

    SETTER(int, AttackPerChain) { ReflectFields->AttackPerChain = std::clamp(value, -99, 99); }
    GETTER(int, AttackPerChain) { return ReflectFields->AttackPerChain; }
    //int 공격 당 연격 부여량
    PROPERTY(AttackPerChain)

    SETTER(float, AttackPerChainMultiplier) { ReflectFields->AttackPerChainMultiplier = value; }
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
    int         WeaponID = 0;
    std::string Name = DEFAULT_NAME;
    WeaponType  Type = WeaponType::SWORD;
    WeaponGrade Grade = WeaponGrade::COMMON;
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