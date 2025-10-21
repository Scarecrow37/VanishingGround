#include "pchScripts.h"
#include "AttackTokenApplyAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/TurnActionFactory.h>
#include <TurnSystem/TurnSystemHelper.h>
#include <Token/TokenSystem.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Stats/Weapon/WeaponStats.h>
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>

REGISTER_TURN_ACTION(AttackTokenApplyAction)

const std::string& AttackTokenApplyAction::GetActionInfo()
{
    return _actionInfo;
}

void AttackTokenApplyAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
}

const std::string& AttackTokenApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"공격시 대상에게 토큰 부여";
    return name;
}

void AttackTokenApplyAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats)
{
    if (EvaluateConditions())
    {
        std::vector<CharacterBase*> targets =TurnSystemHelper::GetTargetCharacters(ReflectFields->TokenTarget);
        if (false == targets.empty())
        {
            for (auto& applyTarget : targets)
            {
                applyTarget->GetTokenInventory().AddTokenStackFromID(TokenID, ReflectFields->TokenCount);
                std::string msg(applyTarget->gameObject->ToString());
                msg += (const char*)u8"에게 ";
                msg += std::format("{}{}{}{}", TokenSystem::GetTokenNameFromID(ReflectFields->TokenID),  (const char*)u8"토큰 ", ReflectFields->TokenCount, (const char*)u8"개 부여");
                UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
            }
        } 
    }
}

void AttackTokenApplyAction::UpdateActionInfo() 
{
    std::string_view tokenName = TokenSystem::GetTokenNameFromID(ReflectFields->TokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"공격시 ";
    _actionInfo += std::format("{}{}{}{}{}{}", rfl::enum_to_string(ReflectFields->TokenTarget), (const char*)u8"에게 ", tokenName, (const char*)u8"토큰 ", ReflectFields->TokenCount, (const char*)u8"개 부여");
}