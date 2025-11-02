#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>
#include "WeaponSystem/WeaponElement/WeaponElement.h"

//특정 종류에 대한 무기 스텟 항상 증가
class WeaponTypeStatsAction : public TurnAction
{
    USING_PROPERTY(WeaponTypeStatsAction)
public:
    inline static const std::string NAME = (const char*)u8"특정 종류에 대한 무기 스텟 항상 증가";

    WeaponTypeStatsAction();
    ~WeaponTypeStatsAction() override = default;

public:
    REFLECT_PROPERTY(
        ReflectFields->Type,
        ReflectFields->HitDamage,
        ReflectFields->HitDamageMultiplier,
        ReflectFields->CriticalDamage,
        ReflectFields->CriticalDamageMultiplier,
        ReflectFields->Speed,
        ReflectFields->AttackPerChain,
        ReflectFields->AttackPerChainMultiplier
    )
protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    WeaponType Type                     = WeaponType::SWORD;
    int        HitDamage                = 0;
    float      HitDamageMultiplier      = 1.f;
    int        CriticalDamage           = 0;
    float      CriticalDamageMultiplier = 1.f;
    int        Speed                    = 0;
    int        AttackPerChain           = 0;
    float      AttackPerChainMultiplier = 1.f;
    REFLECT_FIELDS_END(WeaponTypeStatsAction)

    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnPlayerBattleCalculateChainModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                              Enemy& target, EnemyStats& targetStats) override;

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats) override;

    void OnWeaponRoundSpeedApply(WeaponElement& weapon, int& plusSpeed) override;

    void OnConvertWeaponViewModel(WeaponStats& stats) override;
};