#include "pchScripts.h"
#include "AttackTokenRemoveAction.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

REGISTER_TURN_ACTION(AttackTokenRemoveAction)

void AttackTokenRemoveAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                                    WeaponStats& weaponStats, Enemy& target,
                                                                    EnemyStats& targetStats)
{
    if (EvaluateConditions())
    {
        if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
        {
            std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->TokenTarget);
            if (false == targets.empty())
            {
                for (auto& target : targets)
                {
                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    tokenInventory.RemoveTokenStackFromID(TokenID, ReflectFields->TokenCount);
                }
            }
        }
    }
}


const std::string& AttackTokenRemoveAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void AttackTokenRemoveAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& AttackTokenRemoveAction::GetActionName()
{
    static const std::string name = (const char*)u8"턴 종료시 대상에게 토큰 제거";
    return name;
}

void AttackTokenRemoveAction::UpdateActionInfo()
{
    std::string_view tokenName = TokenSystem::TokenIDToName(ReflectFields->TokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"공격시 ";
    _actionInfo += std::format("{}{}{}{}{}{}", rfl::enum_to_string(ReflectFields->TokenTarget), (const char*)u8"에게 ",
                               tokenName, (const char*)u8"토큰 ", ReflectFields->TokenCount, (const char*)u8"개 제거");
}