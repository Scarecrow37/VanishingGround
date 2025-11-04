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
        
        std::shared_ptr<Component> sharedEnemyComponent = enemy.GetWeakPtr().lock();
        std::weak_ptr<Enemy> weakEnemy = std::static_pointer_cast<Enemy>(sharedEnemyComponent);

        animator->SetCurrentAnimationEndCallback([weakEnemy]() {
            // 사망 애니메이션 종료 시 처리할 내용이 있으면 여기에 작성
            if (auto enemy = weakEnemy.lock())
            {
                const AnimationComponent* animator = enemy->GetAnimationComponent();
                if (animator)
                {
                    enemy->gameObject->SetActive(false);

                    GameObject* monsterHUD = enemy->GetMonsterHUD();
                    if (nullptr != monsterHUD)
                    {
                        monsterHUD->ActiveSelf = false;
                    }
                }
            }
        });
        animator->EndBuildOverrideAnimation();
    }
    Enemy& enemy = GetEnemy();
    if (ParticleComponent* particle = enemy.GetParticleComponent())
    {
        particle->StopAll();
    }
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
