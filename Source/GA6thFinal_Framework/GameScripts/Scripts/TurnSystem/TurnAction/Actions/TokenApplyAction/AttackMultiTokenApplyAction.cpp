#include "pchScripts.h"
#include "AttackMultiTokenApplyAction.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnSystemHelper.h"

REGISTER_TURN_ACTION(AttackMultiTokenApplyAction)

const std::string& AttackMultiTokenApplyAction::GetActionName()
{
    using namespace u8_literals;
    const static std::string name = u8"공격시 토큰 여러개 부여"_c_str;
    return name;
}

const std::string& AttackMultiTokenApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void AttackMultiTokenApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

void AttackMultiTokenApplyAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                                        WeaponStats& weaponStats, Enemy& target,
                                                                        EnemyStats& targetStats)
{
    if (EvaluateConditions())
    {
        std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(Target);
        if (false == targets.empty())
        {
            for (auto& target : targets)
            {
                for (auto& [id, count] : ReflectFields->Tokens)
                {
                    target->GetTokenInventory().AddTokenStackFromID(id, count);
                }
            }
        }
    }
}

void AttackMultiTokenApplyAction::UpdateActionInfo()
{
    using namespace u8_literals;
    _actionInfo.clear();
    _actionInfo = u8"공격시 "_c_str;
    _actionInfo += (const char*)TurnSystemHelper::GetTurnTargetToolTip(Target).data();
    _actionInfo += u8"에게 "_c_str;
    for (auto& [id, count] : ReflectFields->Tokens)
    {
        _actionInfo += TokenSystem::TokenIDToName(id);
        _actionInfo += u8"토큰을 "_c_str;
        _actionInfo += std::to_string(count);
        _actionInfo += u8"개, "_c_str;
    }
    _actionInfo += u8"부여"_c_str;
}
