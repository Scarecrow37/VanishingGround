#include "pchScripts.h"
#include "WeaponTypeStatsAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REFLECT_FUNCTION(WeaponTypeStatsAction)
REGISTER_TURN_ACTION(WeaponTypeStatsAction)

WeaponTypeStatsAction::WeaponTypeStatsAction() = default;

const std::string& WeaponTypeStatsAction::GetActionName()
{
    return NAME;
}

const std::string& WeaponTypeStatsAction::GetActionInfo()
{
    return NAME;
}

void WeaponTypeStatsAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}

void WeaponTypeStatsAction::OnPlayerBattleCalculateChainModifier(Player& attacker, PlayerStats& attackerStats,
                                                                 WeaponStats& weaponStats, Enemy& target,
                                                                 EnemyStats& targetStats)
{
    if (weaponStats.Type == ReflectFields->Type)
    {
        if (EvaluateConditions())
        {
            // 연격만 증가
            int chain = ReflectFields->AttackPerChain;
            float multiplier = ReflectFields->AttackPerChainMultiplier;

            weaponStats.AttackPerChain += chain;
            weaponStats.AttackPerChainMultiplier *= multiplier;
        }
    }
}

void WeaponTypeStatsAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                                  WeaponStats& weaponStats, Enemy& target,
                                                                  EnemyStats& targetStats)
{
    if (weaponStats.Type == ReflectFields->Type)
    {
        if (EvaluateConditions())
        {
            // 데미지만 증가
            int hitDamage = ReflectFields->HitDamage;
            float hitMultiplier = ReflectFields->HitDamageMultiplier;
            weaponStats.HitDamage += hitDamage;
            weaponStats.HitDamageMultiplier *= hitMultiplier;

            int criticalDamage = ReflectFields->CriticalDamage;
            float criticalMultiplier = ReflectFields->CriticalDamageMultiplier;
            weaponStats.CriticalDamage += criticalDamage;
            weaponStats.CriticalDamageMultiplier *= criticalMultiplier;
        }
    }
}

void WeaponTypeStatsAction::OnWeaponRoundSpeedApply(WeaponElement& weapon, int& plusSpeed) 
{
    if (weapon.Stats.Type == ReflectFields->Type)
    {
        if (EvaluateConditions())
        {
            //속도 증가 (UI도 적용됨)
            plusSpeed += ReflectFields->Speed;
        }
    }
}

void WeaponTypeStatsAction::OnConvertWeaponViewModel(WeaponStats& stats) 
{
    if (0 == ConditionCount) //상시 적용만 UI 적용
    {
        if (stats.Type == ReflectFields->Type)
        {
            // UI 적용
            stats.HitDamage += ReflectFields->HitDamage;
            stats.CriticalDamage += ReflectFields->CriticalDamage;
        }
    }
}
