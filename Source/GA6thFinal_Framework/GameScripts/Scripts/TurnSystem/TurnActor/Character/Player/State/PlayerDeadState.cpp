#include "pchScripts.h"
#include "PlayerDeadState.h"
#include <GameCore/FSM/Factory/FSMStateFactory.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Animation/AnimationComponent.h>

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
        player.SetMainAnimation(CharacterBase::DEATH, ANIMATION_FLAG_NONE);
        animator->EndBuildOverrideAnimation();
    }
}

void PlayerDeadState::OnExit() 
{

}

void PlayerDeadState::OnUpdate() 
{

}
