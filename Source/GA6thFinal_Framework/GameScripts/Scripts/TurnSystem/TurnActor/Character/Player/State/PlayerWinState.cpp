#include "pchScripts.h"
#include "PlayerWinState.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "Animation/AnimationComponent.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

REGISTER_CLASS(FSMStateFactory, PlayerWinState)

PlayerWinState::PlayerWinState() {}

PlayerWinState::~PlayerWinState() {}

void PlayerWinState::OnAwake() {}

void PlayerWinState::OnStart() {}

void PlayerWinState::OnEnter()
{
    Player& player   = GetPlayer();
    auto*   animator = player.GetAnimationComponent();
    if (animator)
    {
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();

        // 메인 애니메이션을 Idle로 바꿈
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME);
        animator->ChangeMainAnimation("Idle");

        animator->EndBuildOverrideAnimation();
    }
}

void PlayerWinState::OnUpdate() {}

void PlayerWinState::OnExit() {}

