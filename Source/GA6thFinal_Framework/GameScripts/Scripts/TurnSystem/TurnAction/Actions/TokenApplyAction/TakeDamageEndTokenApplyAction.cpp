#include "pchScripts.h"
#include "TakeDamageEndTokenApplyAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

REGISTER_TURN_ACTION(TakeDamageEndTokenApplyAction)

TakeDamageEndTokenApplyAction::TakeDamageEndTokenApplyAction() 
{
    UpdateActionInfo();
}

const std::string& TakeDamageEndTokenApplyAction::GetActionInfo()
{
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
    _actionInfo = (const char*)TurnSystemHelper::GetTurnTargetToolTip(Target).data();
    _actionInfo += u8" 데미지 입을때 "_c_str;
    _actionInfo += TokenSystem::TokenIDToName(TokenID);
    _actionInfo += u8"토큰을 "_c_str;
    _actionInfo += std::to_string(TokenCount);
    _actionInfo += u8"개 부여"_c_str;
}

void TakeDamageEndTokenApplyAction::OnPlayerTakeDamageEnd(Player& target, int damage) 
{
    std::vector<CharacterBase*> targets =  TurnSystemHelper::GetTargetCharacters(Target);
    CharacterBase* player = &target;
    bool isTarget = false;
    for (auto& targetCharacter : targets)
    {    
        if (targetCharacter == player)
        {
            isTarget = true;
            break;
        }
    }

    if (isTarget && EvaluateConditions())
    {
        target.GetTokenInventory().AddTokenStackFromID(TokenID, TokenCount);
    }
}

void TakeDamageEndTokenApplyAction::OnEnemyTakeDamageEnd(Enemy& target, int damage) 
{
    std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(Target);
    CharacterBase* enemy = &target;
    bool isTarget = false;
    for (auto& targetCharacter : targets)
    {
        if (targetCharacter == enemy)
        {
            isTarget = true;
            break;
        }
    }

    if (isTarget && EvaluateConditions())
    {
        target.GetTokenInventory().AddTokenStackFromID(TokenID, TokenCount);
    }
}
