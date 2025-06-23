#include "EnemyActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

REGISTER_CLASS(FSMStateFactory, EnemyActionPhase)

EnemyActionPhase::EnemyActionPhase() {}

EnemyActionPhase::~EnemyActionPhase() {}

void EnemyActionPhase::OnAwake() {}

void EnemyActionPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void EnemyActionPhase::OnEnter() 
{
    TurnActor* enemy = _turnMode->GetCurrTurnActor();
    enemy->PlayTurn();
}

void EnemyActionPhase::OnExit() {}

void EnemyActionPhase::OnUpdate() {}
