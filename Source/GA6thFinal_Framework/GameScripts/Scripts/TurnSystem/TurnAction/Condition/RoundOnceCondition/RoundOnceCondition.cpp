#include "pchScripts.h"
#include "RoundOnceCondition.h"
#include <TurnSystem/TurnMode/TurnMode.h>

REGISTER_TURN_ACTION_CONDITION(RoundOnceTrueCondition)

bool RoundOnceTrueCondition::Evaluate()
{
    if (_roundOnceAction.Once == false)
    {      
        return true;
    }
    return false;
}

void RoundOnceTrueCondition::DrawImguiEditor() 
{

}

const std::string& RoundOnceTrueCondition::GetConditionInfo() const
{
    static const std::string info = (const char*)u8"라운드당 한번만 발동";
    return info;
}

void RoundOnceTrueCondition::OnEvaluateConditions(bool result) 
{
    if (true == result)
    {
        _roundOnceAction.Once = true;
    }
}

RoundOnceTrueCondition::RoundOnceAction::RoundOnceAction()
{
    TurnMode* turnMode = TurnMode::GetInstance();
    if (turnMode)
    {
        turnMode->AddTurnAction(this);
    }
}

RoundOnceTrueCondition::RoundOnceAction::~RoundOnceAction()
{
    SetDestroy();
}

const std::string& RoundOnceTrueCondition::RoundOnceAction::GetActionName()
{
    return NAME;
}

const std::string& RoundOnceTrueCondition::RoundOnceAction::GetActionInfo()
{
    return NAME;
}

void RoundOnceTrueCondition::RoundOnceAction::ImGuiDrawActionEditor() 
{

}

void RoundOnceTrueCondition::RoundOnceAction::OnRoundStart()
{
    Once = false;
}
