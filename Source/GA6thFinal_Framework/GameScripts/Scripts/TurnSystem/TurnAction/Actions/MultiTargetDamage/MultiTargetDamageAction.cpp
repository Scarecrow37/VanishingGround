#include "pchScripts.h"
#include "MultiTargetDamageAction.h"

REGISTER_TURN_ACTION(MultiTargetDamageAction)

MultiTargetDamageAction::MultiTargetDamageAction() 
{
    UpdateActionInfo();
}

const std::string& MultiTargetDamageAction::GetActionName()
{
    static const std::string actionName = (const char*)u8"공격시 모든 적을 공격";
    return actionName;
}

const std::string& MultiTargetDamageAction::GetActionInfo()
{
    return _actionInfo;
}

void MultiTargetDamageAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}

void MultiTargetDamageAction::OnPlayerBattleTargetSelected(Battle::EnemyTargetFlag& targetFlag) 
{
    if (EvaluateConditions())
    {
        targetFlag = Battle::ENEMY_TARGET_FLAG_ALL;
    }
}

void MultiTargetDamageAction::UpdateActionInfo() 
{
    _actionInfo = (const char*)u8"플레이어가 모든 적을 공격합니다.";
}
