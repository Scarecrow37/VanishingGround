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
        // 이미 Idle인 상태면 패스
        const std::string& key = animator->GetAnimationNameFromKey("Idle");
        if (false == animator->GetMainAnimationData().IsSameAnimation(key))
        {
            animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME |
                                            ANIMATION_FLAG_USE_BLEND);
            animator->ChangeMainAnimation("Idle");
        }
    }
}

void EnemyWaitTurnState::OnExit() {}

void EnemyWaitTurnState::OnUpdate() {}