#include "pchScripts.h"
#include "PlayerActionCondition.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

REGISTER_CLASS(FSMConditionFactory, PlayerActionCondition)

PlayerActionCondition::PlayerActionCondition() {}

PlayerActionCondition::~PlayerActionCondition() {}

void PlayerActionCondition::OnAwake() 
{
    _turnMode = GetFSM().GetComponent<TurnMode>();
}

void PlayerActionCondition::OnStart() 
{
}

bool PlayerActionCondition::Evaluate()
{
    if (auto& currTurnModel = _turnMode->GetCurrTurnActor(); nullptr != currTurnModel)
    {
        if (typeid(Player) == typeid(*currTurnModel))
        {
            return true;
        }
    }
    return false;
}
