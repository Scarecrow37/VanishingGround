#include "CombatStartTokenRandomApplyAction.h"
#include "pchScripts.h"

#include "Token/TokenSystem.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnSystemHelper.h"

REGISTER_TURN_ACTION(CombatStartTokenRandomApplyAction)

const std::string& CombatStartTokenRandomApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void CombatStartTokenRandomApplyAction::ImGuiDrawActionEditor()
{
    ImGuiDrawPropertys();
}

const std::string& CombatStartTokenRandomApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"전투 시작시 대상에게 토큰 랜덤하게 부여";
    return name;
}

void CombatStartTokenRandomApplyAction::OnCombatStart()
{
    if (EvaluateConditions())
    {
        const int        tokenID       = ReflectFields->TokenID;
        const int        tokenCountMin = ReflectFields->TokenCountMin;
        const int        tokenCountMax = ReflectFields->TokenCountMax;
        const TurnTarget target        = ReflectFields->TokenTarget;

        if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
        {
            std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(target);
            if (false == targets.empty())
            {
                for (auto& target : targets)
                {
                    TokenInventory& tokenInventory = target->GetTokenInventory();
                    const int       randomIndex    = Random::Range(tokenCountMin, tokenCountMax);
                    tokenInventory.AddTokenStackFromID(TokenID, randomIndex);
                }
            }
        }
    }
}

void CombatStartTokenRandomApplyAction::UpdateActionInfo()
{
    const int tokenID       = ReflectFields->TokenID;
    const int tokenCountMin = ReflectFields->TokenCountMin;
    const int tokenCountMax = ReflectFields->TokenCountMax;
    const TurnTarget target = ReflectFields->TokenTarget;

    std::string_view tokenName = TokenSystem::TokenIDToName(tokenID);
    if (true == tokenName.empty())
    {
        tokenName = STR_NULL;
    }
    _actionInfo = (const char*)u8"전투 시작시 ";
    _actionInfo += std::format("{}{}{}{}{}{}{}{}", 
        rfl::enum_to_string(target), 
        (const char*)u8"에게 ",
        tokenName, 
        (const char*)u8"토큰 ", 
        tokenCountMin,
        " ~ ", 
        tokenCountMax,
        (const char*)u8"개 부여");
}