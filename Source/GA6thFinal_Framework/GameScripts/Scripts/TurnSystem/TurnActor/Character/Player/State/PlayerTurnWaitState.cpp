#include "pchScripts.h"
#include "PlayerTurnWaitState.h"

#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

REGISTER_CLASS(FSMStateFactory, PlayerTurnWaitState)

PlayerTurnWaitState::PlayerTurnWaitState() 
{

}

PlayerTurnWaitState::~PlayerTurnWaitState() 
{

}

bool PlayerTurnWaitState::IsMyTurn()
{
    return GetPlayer().IsMyTurn;
}

void PlayerTurnWaitState::OnAwake() 
{

}

void PlayerTurnWaitState::OnStart() 
{

}

void PlayerTurnWaitState::OnEnter() 
{

}

void PlayerTurnWaitState::OnExit() 
{

}

void PlayerTurnWaitState::OnUpdate() 
{

}
