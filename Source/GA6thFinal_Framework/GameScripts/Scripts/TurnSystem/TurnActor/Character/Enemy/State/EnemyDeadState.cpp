#include "pchScripts.h"
#include "EnemyDeadState.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>
#include <Audio/Table/AudioTableComponent.h>

REGISTER_CLASS(FSMStateFactory, EnemyDeadState)

void EnemyDeadState::OnAwake() {}

void EnemyDeadState::OnStart() {}

void EnemyDeadState::OnEnter() 
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"사망.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
    Enemy& enemy = GetEnemy();
    enemy.Dead();
    AnimationComponent* animator = enemy.GetAnimationComponent();
    AudioTableComponent* audioTable = enemy.GetAudioTableComponent();
    if (animator && false == _dontChangeAnimation)
    {
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();
        animator->ChangeMainAnimation("Dead", true);
        animator->ChangeMainAnimationFlags(ANIMATION_FLAG_NONE);
        animator->EndBuildOverrideAnimation();
        if (audioTable)
        {
            audioTable->Play("Dead0");
        }
    }
}

void EnemyDeadState::OnExit() 
{
    _dontChangeAnimation = false;
}

void EnemyDeadState::OnUpdate() 
{
    Enemy& enemy = GetEnemy();
    AnimationComponent* animator = enemy.GetAnimationComponent();
    if (animator)
    {
        if (animator->GetMainAnimationData().IsEnd())
        {
            enemy.gameObject->SetActive(false);
        }
    }
}

void EnemyDeadState::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
}
