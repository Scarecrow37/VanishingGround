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
    TurnActor* currTurn = _turnMode->GetCurrTurnActor();
    if (nullptr != currTurn)
    {
        if (typeid(Player) == typeid(*currTurn))
        {
            return true;
        }
    }
    return false;
}
