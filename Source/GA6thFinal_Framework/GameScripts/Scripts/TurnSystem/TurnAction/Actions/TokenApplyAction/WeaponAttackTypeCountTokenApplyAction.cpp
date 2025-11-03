#include "pchScripts.h"
#include "WeaponAttackTypeCountTokenApplyAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

REFLECT_FUNCTION(WeaponAttackTypeCountTokenApplyAction)
REGISTER_TURN_ACTION(WeaponAttackTypeCountTokenApplyAction)

const std::string& WeaponAttackTypeCountTokenApplyAction::GetActionInfo()
{
    return NAME;
}

void WeaponAttackTypeCountTokenApplyAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
}

const std::string& WeaponAttackTypeCountTokenApplyAction::GetActionName()
{
    return NAME;
}

void WeaponAttackTypeCountTokenApplyAction::OnEquipAccessory() 
{
    _attackCount = 0;
}

void WeaponAttackTypeCountTokenApplyAction::OnPlayerBattleCalculateDamageModifier(
    Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats)
{
    if (weaponStats.Type == ReflectFields->WeaponTargetType)
    {
        if (EvaluateConditions())
        {
            ++_attackCount;
            if (_attackCount == ReflectFields->TargetAttackCount)
            {
                auto tokenApplyTargets = TurnSystemHelper::GetTargetCharacters(Target);
                for (auto& tokenTarget : tokenApplyTargets)
                {
                    if (tokenTarget)
                    {
                        auto& inventory = tokenTarget->GetTokenInventory();
                        inventory.AddTokenStackFromID(TokenID, TokenCount);
                    }
                }
                _attackCount = 0;
            }
        }
    }
}
