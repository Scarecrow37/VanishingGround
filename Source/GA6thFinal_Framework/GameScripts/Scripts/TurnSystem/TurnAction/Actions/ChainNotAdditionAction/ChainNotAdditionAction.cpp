#include "pchScripts.h"
#include "ChainNotAdditionAction.h"
#include "Stats/Weapon/WeaponStats.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REGISTER_TURN_ACTION(ChainNotAdditionAction)

const std::string& ChainNotAdditionAction::GetActionName()
{
    return _name;
}

const std::string& ChainNotAdditionAction::GetActionInfo()
{
    return _name;
}

void ChainNotAdditionAction::ImGuiDrawActionEditor() 
{
    ImguiDrawConditionEditor();
}

void ChainNotAdditionAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                                   WeaponStats& weaponStats, Enemy& target,
                                                                   EnemyStats& targetStats)
{
    if (EvaluateConditions())
    {
        weaponStats.AttackPerChain = -100;
        weaponStats.AttackPerChainMultiplier = std::numeric_limits<int>::min();
    }
}
