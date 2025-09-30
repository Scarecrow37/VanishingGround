#include "pchScripts.h"
#include "EnemyDeadState.h"

#include "UI/Views/MonsterHp/MonsterHpView.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

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
    if (animator && false == _dontChangeAnimation)
    {
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND);
        animator->ChangeMainAnimation("Dead");
        animator->EndBuildOverrideAnimation();
    }
}

void EnemyDeadState::OnExit() 
{
    _dontChangeAnimation = false;
}

void EnemyDeadState::OnUpdate() 
{
    const Enemy& enemy = GetEnemy();
    const AnimationComponent* animator = enemy.GetAnimationComponent();
    if (animator)
    {
        if (animator->GetMainAnimationData().IsEnd())
        {
            enemy.gameObject->SetActive(false);

            GameObject* monsterHUD = enemy.GetMonsterHUD();
            if (nullptr != monsterHUD)
                monsterHUD->ActiveSelf = false;
        }
    }
}

void EnemyDeadState::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
}
