#include "pchScripts.h"
#include "EnemyWaitTurnState.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, EnemyWaitTurnState)

EnemyWaitTurnState::EnemyWaitTurnState() 
{

}

EnemyWaitTurnState::~EnemyWaitTurnState() 
{}

bool EnemyWaitTurnState::IsMyTurn()
{
    return GetEnemy().IsMyTurn;
}

void EnemyWaitTurnState::OnAwake() {}

void EnemyWaitTurnState::OnStart() {}

void EnemyWaitTurnState::OnEnter() {}

void EnemyWaitTurnState::OnExit() {}

void EnemyWaitTurnState::OnUpdate() {}