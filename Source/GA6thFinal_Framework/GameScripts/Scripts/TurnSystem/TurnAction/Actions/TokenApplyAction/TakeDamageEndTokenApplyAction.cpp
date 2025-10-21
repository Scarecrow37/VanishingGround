#include "pchScripts.h"
#include "TakeDamageEndTokenApplyAction.h"

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
    _actionInfo.clear();

}

void TakeDamageEndTokenApplyAction::OnPlayerTakeDamageEnd(Player& target, int damage) 
{

}

void TakeDamageEndTokenApplyAction::OnEnemyTakeDamageEnd(Enemy& target, int damage) 
{

}
