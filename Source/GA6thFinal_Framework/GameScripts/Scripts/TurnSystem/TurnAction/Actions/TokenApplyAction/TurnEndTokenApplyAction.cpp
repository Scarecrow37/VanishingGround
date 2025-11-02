#include "pchScripts.h"
#include "TurnEndTokenApplyAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

REGISTER_TURN_ACTION(TurnEndTokenApplyAction)

const std::string& TurnEndTokenApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void TurnEndTokenApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& TurnEndTokenApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"턴 종료시 대상에게 토큰 부여";
    return name;
}

void TurnEndTokenApplyAction::OnTurnEnd(CharacterBase& destination)
{
    if (typeid(destination) == typeid(Player))
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
                        tokenInventory.AddTokenStackFromID(TokenID, ReflectFields->TokenCount);
                    }
                }
            }
        }
    }
}

void TurnEndTokenApplyAction::UpdateActionInfo()
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
        (const char*)u8"개 부여");
}