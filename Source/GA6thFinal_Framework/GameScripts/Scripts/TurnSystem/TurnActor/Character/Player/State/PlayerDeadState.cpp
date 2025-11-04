#include "pchScripts.h"
#include "PlayerDeadState.h"
#include <GameCore/FSM/Factory/FSMStateFactory.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>

REGISTER_CLASS(FSMStateFactory, PlayerDeadState)

PlayerDeadState::PlayerDeadState() 
{

}

PlayerDeadState::~PlayerDeadState() 
{

}

void PlayerDeadState::OnAwake() 
{

}

void PlayerDeadState::OnStart() 
{

}

void PlayerDeadState::OnEnter() 
{
    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"플레이어 사망!!!");
    Player& player = GetPlayer();
    player.Dead();
    AnimationComponent* animator = player.GetAnimationComponent();
    if (animator)
    {
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND);
        animator->ChangeMainAnimation("Dead");
        animator->EndBuildOverrideAnimation();
    }
    Player& enemy = GetPlayer();
    if (ParticleComponent* particle = enemy.GetParticleComponent())
    {
        particle->StopAll();
    }
}

void PlayerDeadState::OnExit() 
{
}

void PlayerDeadState::OnUpdate() 
{
    Player&             player   = GetPlayer();
    AnimationComponent* animator = player.GetAnimationComponent();
    if (animator)
    {
        if (animator->GetMainAnimationData().IsEnd())
        {
            player.gameObject->SetActive(false);
        }
    }
}
