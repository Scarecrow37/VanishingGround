#include "pchScripts.h"
#include "PlayerWaitTurnState.h"

#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

REGISTER_CLASS(FSMStateFactory, PlayerWaitTurnState)

PlayerWaitTurnState::PlayerWaitTurnState() 
{

}

PlayerWaitTurnState::~PlayerWaitTurnState() 
{

}

bool PlayerWaitTurnState::IsMyTurn()
{
    return GetPlayer().IsMyTurn;
}

void PlayerWaitTurnState::OnAwake() 
{

}

void PlayerWaitTurnState::OnStart() 
{

}

void PlayerWaitTurnState::OnEnter() 
{
    auto& player = GetPlayer();
    player.SetMainAnimation(CharacterBase::IDLE, ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME);
}

void PlayerWaitTurnState::OnExit() 
{

}

void PlayerWaitTurnState::OnUpdate() 
{

}
