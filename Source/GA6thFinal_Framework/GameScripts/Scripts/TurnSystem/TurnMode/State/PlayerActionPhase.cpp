#include "PlayerActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

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
    GetTurnModeBase::OnStart();
}

void PlayerActionPhase::OnEnter() 
{
    TurnActor* player = _turnMode->GetCurrTurnActor();
    player->PlayTurn();
}

void PlayerActionPhase::OnExit() {}

void PlayerActionPhase::OnUpdate() {}
