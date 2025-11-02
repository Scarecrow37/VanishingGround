#include "pchScripts.h"
#include "TakeDamageEndTokenApplyAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_TURN_ACTION(TakeDamageEndTokenApplyAction)

const std::string& TakeDamageEndTokenApplyAction::GetActionInfo()
{
    TryTokenSystemInfoUpdate();
    return _actionInfo;
}

void TakeDamageEndTokenApplyAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
}

const std::string& TakeDamageEndTokenApplyAction::GetActionName()
{
    const static std::string actionName = (const char*)u8"데미지 입을시 토큰 부여";
    return actionName;
}

void TakeDamageEndTokenApplyAction::UpdateActionInfo() 
{
    using namespace u8_literals;
    _actionInfo.clear();
    _actionInfo += u8"플레이어가 데미지 입을때 "_c_str;
    _actionInfo += (const char*)TurnSystemHelper::GetTurnTargetToolTip(Target).data();
    _actionInfo += u8" 에게 "_c_str;
    _actionInfo += TokenSystem::TokenIDToName(TokenID);
    _actionInfo += u8"토큰을 "_c_str;
    _actionInfo += std::to_string(TokenCount);
    _actionInfo += u8"개 부여"_c_str;
}

void TakeDamageEndTokenApplyAction::OnPlayerTakeDamageEnd(Player& player, int damage) 
{
    if (EvaluateConditions())
    {
        std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(Target);
        for (auto& target : targets)
        {
            if (target)
            {
                target->GetTokenInventory().AddTokenStackFromID(TokenID, TokenCount);
            }          
        } 
    }
}
