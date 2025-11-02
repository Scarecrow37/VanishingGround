#include "pchScripts.h"
#include "RoundStartTokenApplyAction.h"

#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnSystemHelper.h"

REGISTER_TURN_ACTION(RoundStartTokenApplyAction)

const std::string& RoundStartTokenApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void RoundStartTokenApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& RoundStartTokenApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"라운드 시작시 대상에게 토큰 부여";
    return name;
}

void RoundStartTokenApplyAction::OnRoundStart()
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

void RoundStartTokenApplyAction::UpdateActionInfo()
{
    std::string_view tokenName = TokenSystem::TokenIDToName(ReflectFields->TokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"라운드 시작시 ";
    _actionInfo += std::format("{}{}{}{}{}{}", rfl::enum_to_string(ReflectFields->TokenTarget), (const char*)u8"에게 ",
                               tokenName, (const char*)u8"토큰 ", ReflectFields->TokenCount, (const char*)u8"개 부여");
}