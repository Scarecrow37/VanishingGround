#include "pchScripts.h"
#include "EnemyDeadState.h"
#include "UI/Views/MonsterHp/MonsterHpView.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>
#include "KeyCallbackUINavi/KeyCallbackUINavi.h"

REGISTER_CLASS(FSMStateFactory, EnemyDeadState)

void EnemyDeadState::OnAwake() {}

void EnemyDeadState::OnStart() {}

void EnemyDeadState::OnEnter() 
{
    Enemy& enemy = GetEnemy();
    std::string message = std::format("{} {}", enemy.gameObject->ToString(), (const char*)u8"사망.");
    UmLogger.Message(LogLevel::LEVEL_DEBUG, message);

    float deactiveDelay = 2.0f;

    if (false == _dontChangeAnimation)
    {
        if (AnimationComponent* animator = enemy.GetAnimationComponent())
        {
            animator->BeginBuildOverrideAnimation();
            animator->ClearOverrideAnimations();
            animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND);
            animator->ChangeMainAnimation("Dead");
            animator->EndBuildOverrideAnimation();

            deactiveDelay = animator->GetMainAnimationData().GetAnimationMaxFrame();
        }
    }

    if (ParticleComponent* particle = enemy.GetParticleComponent())
    {
        particle->StopAll();
    }

    UmTime.Invoke(GetFSM(), deactiveDelay, [this]() {
        Enemy& enemy = GetEnemy();
        enemy.gameObject->SetActive(false);
        if (GameObject* monsterHUD = enemy.GetMonsterHUD())
        {
            monsterHUD->ActiveSelf = false;
            Monster::SpawnPoint point = enemy.SpawnPoint;
            KeyCallbackUINavi*  navi  = nullptr;
            switch (point)
            {
            case Monster::SpawnPoint::Invalid:
                break;
            case Monster::SpawnPoint::Left:
                if (auto callbackNavi = GameObject::FindComponentWithTag<KeyCallbackUINavi>("Enemy Left Panel UI Navi").lock())
                {
                    navi = callbackNavi.get();
                }
                break;
            case Monster::SpawnPoint::Middle:
                if (auto callbackNavi = GameObject::FindComponentWithTag<KeyCallbackUINavi>("Enemy Middle Panel UI Navi").lock())
                {
                    navi = callbackNavi.get();
                }
                break;
            case Monster::SpawnPoint::Right:
                if (auto callbackNavi = GameObject::FindComponentWithTag<KeyCallbackUINavi>("Enemy Right Panel UI Navi").lock())
                {
                    navi = callbackNavi.get();
                }
                break;
            default:
                break;
            }
            if (navi)
            {
                navi->Enable = false;
            }
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
