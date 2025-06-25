#include "pchScripts.h"
#include "PlayerStartCondition.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

REGISTER_CLASS(FSMConditionFactory, PlayerStartCondition)

PlayerStartCondition::PlayerStartCondition() {}

PlayerStartCondition::~PlayerStartCondition() {}

void PlayerStartCondition::OnAwake() {}

void PlayerStartCondition::OnStart() {}

bool PlayerStartCondition::Evaluate()
{
    return true == GetPlayer().IsMyTurn;
}
