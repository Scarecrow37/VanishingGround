#include "PlayerActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

REGISTER_CLASS(FSMStateFactory, PlayerActionPhase)

PlayerActionPhase::PlayerActionPhase() 
{
}

PlayerActionPhase::~PlayerActionPhase() 
{
}

void PlayerActionPhase::OnAwake() 
{
}

void PlayerActionPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void PlayerActionPhase::OnEnter() 
{
    TurnActor* player = _turnMode->GetCurrTurnActor();
    player->PlayTurn();
}

void PlayerActionPhase::OnExit() {}

void PlayerActionPhase::OnUpdate() {}
