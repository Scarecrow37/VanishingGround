#include "pchScripts.h"
#include "PlayerDeadState.h"
#include <GameCore/FSM/Factory/FSMStateFactory.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>
#include <UI/Elements/SpriteAnimation/SpriteAnimationElement.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
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
    if (auto object = GameObject::FindWithTag("Vanished Panel").lock())
    {
        _vanishedOverlay = object->GetComponent<OverlayPanel>();
    }
    if (auto object = GameObject::FindWithTag("Vanished Animation").lock())
    {
        _vanishedAnimation = object->GetComponent<SpriteAnimationElement>();
    }
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
    if (_vanishedOverlay)
    {
        _vanishedOverlay->gameObject->ActiveSelf = true;
    }
    if (_vanishedAnimation)
    {
        _vanishedAnimation->Setup();
        _vanishedAnimation->StartAnimation();
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
