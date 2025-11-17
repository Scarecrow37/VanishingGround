#include "pchScripts.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "WeaponTypeCountAttackDamageAction.h"

#include "WeaponSystem/WeaponSystem.h"

REGISTER_TURN_ACTION(WeaponTypeCountAttackDamageAction)
REFLECT_FUNCTION(WeaponTypeCountAttackDamageAction)

int WeaponTypeCountAttackDamageAction::CaculateHitDamage()
{
    int count = 0;
    if (WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance())
    {
        count = system->GetEquipWeaponTypeCount(ReflectFields->Type);
    }
    return count * ReflectFields->HitDamage;
}

int WeaponTypeCountAttackDamageAction::CaculateCriticalDamage()
{
    int count = 0;
    if (WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance())
    {
        count = system->GetEquipWeaponTypeCount(ReflectFields->Type);
    }
    return count * ReflectFields->CriticalDamage;
}   

void WeaponTypeCountAttackDamageAction::OnPlayerBattleCalculateDamageModifier(Player&      attacker,
                                                                              PlayerStats& attackerStats,
                                                                              WeaponStats& weaponStats, Enemy& target,
                                                                              EnemyStats& targetStats)
{
    if (weaponStats.Type == ReflectFields->Type)
    {
        if (EvaluateConditions())
        {
            int hitDamage      = CaculateHitDamage();
            int criticalDamage = CaculateCriticalDamage();
            weaponStats.HitDamage += hitDamage;
            weaponStats.CriticalDamage += criticalDamage;
        }
    } 
}

void WeaponTypeCountAttackDamageAction::OnConvertWeaponViewModel(WeaponStats& stats) 
{
    if (0 == ConditionCount)
    {
        if (stats.Type == ReflectFields->Type)
        {
            int hitDamage      = CaculateHitDamage();
            int criticalDamage = CaculateCriticalDamage();
            stats.HitDamage += hitDamage;
            stats.CriticalDamage += criticalDamage;
        }
    }
}

const std::string& WeaponTypeCountAttackDamageAction::GetActionName()
{
    return NAME;
}

const std::string& WeaponTypeCountAttackDamageAction::GetActionInfo()
{
    return NAME;
}

void WeaponTypeCountAttackDamageAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}
