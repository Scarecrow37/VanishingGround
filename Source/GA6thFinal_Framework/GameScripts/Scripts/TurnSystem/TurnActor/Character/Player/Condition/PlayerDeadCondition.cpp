#include "pchScripts.h"
#include "PlayerDeadCondition.h"
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <GameCore/FSM/Factory/FSMConditionFactory.h>

REGISTER_CLASS(FSMConditionFactory, PlayerDeadCondition)

PlayerDeadCondition::PlayerDeadCondition() 
{}

PlayerDeadCondition::~PlayerDeadCondition() 
{}

void PlayerDeadCondition::OnAwake() 
{}

void PlayerDeadCondition::OnStart() 
{}

bool PlayerDeadCondition::Evaluate()
{
    TurnActor::STATE state = GetPlayer().GetActorState();
    return state == TurnActor::STATE::Dead;
}
