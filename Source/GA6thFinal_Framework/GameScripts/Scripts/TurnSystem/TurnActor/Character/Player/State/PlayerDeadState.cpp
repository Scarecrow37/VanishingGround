#include "pchScripts.h"
#include "PlayerDeadState.h"
#include <GameCore/FSM/Factory/FSMStateFactory.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Mesh/SkeletalMeshRenderer.h>

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
    Player& player = GetPlayer();
    player.Dead();
    SkeletalMeshRenderer* renderer = player.GetSkeletalMeshRenderer();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        player.SetMainAnimation(CharacterBase::DEATH, false);
        renderer->EndBuildOverrideAnimation();
    }
}

void PlayerDeadState::OnExit() 
{

}

void PlayerDeadState::OnUpdate() 
{

}
