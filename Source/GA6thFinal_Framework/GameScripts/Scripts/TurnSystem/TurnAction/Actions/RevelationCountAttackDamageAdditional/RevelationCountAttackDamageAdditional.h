#pragma once
#include "TurnSystem/TurnAction/TurnActionFactory.h"

class Player;
struct PlayerStats;
struct WeaponStats;
class Enemy;
struct EnemyStats;

// 보유 계시 n개당 데미지 증가
class RevelationCountAttackDamageAdditional : public TurnAction
{
    USING_PROPERTY(RevelationCountAttackDamageAdditional)
public:
    RevelationCountAttackDamageAdditional();
    ~RevelationCountAttackDamageAdditional() override;

   REFLECT_PROPERTY(RevelationPerCount, AdditionalDamage, AdditionalDamageMultiplier, AdditionalCriticalDamage,
                     AdditionalCriticalDamageMultiplier)

   GETTER(int, RevelationPerCount) { return ReflectFields->RevelationPerCount; }
   SETTER(int, RevelationPerCount)
   {
       ReflectFields->RevelationPerCount = value;
       UpdateActionInfo();
   }
   // int : 보유 계시 n개당
   PROPERTY(RevelationPerCount)

    GETTER(int, AdditionalDamage) { return ReflectFields->AdditionalDamage; }
    SETTER(int, AdditionalDamage)
    {
        ReflectFields->AdditionalDamage = value;
        UpdateActionInfo();
    }
    // int : 피해 증가량
    PROPERTY(AdditionalDamage)

    GETTER(float, AdditionalDamageMultiplier) { return ReflectFields->AdditionalDamageMultiplier; }
    SETTER(float, AdditionalDamageMultiplier)
    {
        ReflectFields->AdditionalDamageMultiplier = value;
        UpdateActionInfo();
    }
    // float : 피해 배율 증가량
    PROPERTY(AdditionalDamageMultiplier)

    GETTER(int, AdditionalCriticalDamage) { return ReflectFields->AdditionalCriticalDamage; }
    SETTER(int, AdditionalCriticalDamage)
    {
        ReflectFields->AdditionalCriticalDamage = value;
        UpdateActionInfo();
    }
    // int : 치명타 피해 증가량
    PROPERTY(AdditionalCriticalDamage)

    GETTER(float, AdditionalCriticalDamageMultiplier) { return ReflectFields->AdditionalCriticalDamageMultiplier; }
    SETTER(float, AdditionalCriticalDamageMultiplier)
    {
        ReflectFields->AdditionalCriticalDamageMultiplier = value;
        UpdateActionInfo();
    }
    // float : 치명타 피해 배율 증가량
    PROPERTY(AdditionalCriticalDamageMultiplier)

protected:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    void               DeserializedReflectEvent() override;

    REFLECT_FIELDS_BEGIN(TurnAction)
    int   RevelationPerCount                 = 1;   // 보유 개시 당 갯수
    int   AdditionalDamage                   = 0;   // 피해 증가량
    float AdditionalDamageMultiplier         = 1.f; // 피해 배율 증가량
    int   AdditionalCriticalDamage           = 0;   // 피해 증가량
    float AdditionalCriticalDamageMultiplier = 1.f; // 피해 배율 증가량
    REFLECT_FIELDS_END(RevelationCountAttackDamageAdditional)

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats) override;

private:
    void UpdateActionInfo();
    std::string _actionInfo;
};