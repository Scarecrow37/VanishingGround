#include "Token/TokenSystem.h"
#include "TurnEndTokenRemoveAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "pchScripts.h"

REGISTER_TURN_ACTION(TurnEndTokenRemoveAction)

const std::string& TurnEndTokenRemoveAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void TurnEndTokenRemoveAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& TurnEndTokenRemoveAction::GetActionName()
{
    static const std::string name = (const char*)u8"턴 종료시 대상에게 토큰 제거";
    return name;
}

void TurnEndTokenRemoveAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
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

void TurnEndTokenRemoveAction::UpdateActionInfo()
{
    std::string_view tokenName = TokenSystem::TokenIDToName(ReflectFields->TokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"턴 종료시 ";
    _actionInfo += std::format("{}{}{}{}{}{}",
        rfl::enum_to_string(ReflectFields->TokenTarget),
        (const char*)u8"에게 ",
        tokenName,
        (const char*)u8"토큰 ", 
        ReflectFields->TokenCount,
        (const char*)u8"개 제거");
}