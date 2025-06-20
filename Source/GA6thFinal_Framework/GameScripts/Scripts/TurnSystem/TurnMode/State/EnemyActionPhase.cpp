#include "EnemyActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

EnemyActionPhase::EnemyActionPhase() {}

EnemyActionPhase::~EnemyActionPhase() {}

void EnemyActionPhase::OnAwake() {}

void EnemyActionPhase::OnStart() 
{
    GetTurnModeBase::OnStart();
}

void EnemyActionPhase::OnEnter() 
{
    TurnActor* enemy = _turnMode->GetCurrTurnActor();
    enemy->PlayTurn();
}

void EnemyActionPhase::OnExit() {}

void EnemyActionPhase::OnUpdate() {}
