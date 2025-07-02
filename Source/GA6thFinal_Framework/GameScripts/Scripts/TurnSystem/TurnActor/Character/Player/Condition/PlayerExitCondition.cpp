#include "pchScripts.h"
#include "PlayerExitCondition.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

REGISTER_CLASS(FSMConditionFactory, PlayerExitCondition)

PlayerExitCondition::PlayerExitCondition() 
{

}

PlayerExitCondition::~PlayerExitCondition() 
{

}

void PlayerExitCondition::OnAwake() {}

void PlayerExitCondition::OnStart() {}

bool PlayerExitCondition::Evaluate()
{
    return GetPlayer().GetActorState() == TurnActor::STATE::Wait;
}
