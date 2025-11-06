#include "pchScripts.h"
#include "EnemyDeadState.h"
#include "UI/Views/MonsterHp/MonsterHpView.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>

REGISTER_CLASS(FSMStateFactory, EnemyDeadState)

void EnemyDeadState::OnAwake() {}

void EnemyDeadState::OnStart() {}

void EnemyDeadState::OnEnter() 
{
    Enemy& enemy = GetEnemy();
    std::string message = std::format("{} {}", enemy.gameObject->ToString(), (const char*)u8"사망.");
    UmLogger.Message(LogLevel::LEVEL_DEBUG, message);

    if (false == _dontChangeAnimation)
    {
        if (AnimationComponent* animator = enemy.GetAnimationComponent())
        {
            animator->BeginBuildOverrideAnimation();
            animator->ClearOverrideAnimations();
            animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND);
            animator->ChangeMainAnimation("Dead");
            animator->EndBuildOverrideAnimation();
        }
    }

    if (ParticleComponent* particle = enemy.GetParticleComponent())
    {
        particle->StopAll();
    }

    UmTime.Invoke(GetFSM(), 2.0f, [this]() {
        Enemy& enemy = GetEnemy();
        enemy.gameObject->SetActive(false);
        if (GameObject* monsterHUD = enemy.GetMonsterHUD())
        {
            monsterHUD->ActiveSelf = false;
        }
    });
    enemy.Dead();
}

void EnemyDeadState::OnExit() 
{
    _dontChangeAnimation = false;
}

void EnemyDeadState::OnUpdate() 
{
}

void EnemyDeadState::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
}
