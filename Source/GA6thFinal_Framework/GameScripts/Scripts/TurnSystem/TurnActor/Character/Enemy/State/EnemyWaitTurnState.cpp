#include "pchScripts.h"
#include "EnemyWaitTurnState.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

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

void EnemyWaitTurnState::OnEnter() 
{
    auto& enemy = GetEnemy();
    auto animator = enemy.GetAnimationComponent();
    if (animator)
    {
        animator->ChangeMainAnimation("Idle", true);
        animator->ChangeMainAnimationFlags(ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME);
    }
}

void EnemyWaitTurnState::OnExit() {}

void EnemyWaitTurnState::OnUpdate() {}