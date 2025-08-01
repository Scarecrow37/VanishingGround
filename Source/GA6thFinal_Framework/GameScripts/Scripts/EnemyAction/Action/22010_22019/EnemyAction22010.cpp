#include "pchScripts.h"
#include "EnemyAction22010.h"
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/Token/TokenApplyAction.h>
#include <Token/Object/Bleed/BleedToken.h>
namespace EnemyAction
{
    Action22010::Action22010(Enemy* owner) 
        : ActionBase(owner), _tokenAction(std::make_unique<TokenApplyAction>())
    {
        _tokenAction->TokenID = TokenObject::Bleed::ID;
        _tokenAction->TokenCount = 1;
    }
    Action22010::~Action22010() 
    {
    }

    void Action22010::OnActionEnter() 
    {
        if (_animator)
        {
            _animator->BeginBuildOverrideAnimation();
            {
                _animator->ClearOverrideAnimations();
                _animator->PushOverrideAnimation("Attack0", true, [](const AnimationData& data) { return data.IsEnd(); });
                _animator->SetCurrentAnimationPopCallback([this]() { SetActionEnd(); });
                _animator->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            }
            _animator->EndBuildOverrideAnimation();
        }
    }

    void EnemyAction::Action22010::OnActionUpdate()
    {
    }

    void Action22010::OnActionExit() 
    {
    }

    void Action22010::OnAnimationEvent(const Timeline::EventContext* context) 
    {
        const std::string& label = context->GetLabel();
        if ("Attack_1" == label)
        {
            TurnMode* turnMode = TurnMode::GetInstance();
            if (turnMode)
            {
                turnMode->AddTurnAction(_tokenAction.get());
                ProcessBattle(5);
                _tokenAction->SetDestroy();
            }
        }
    }
} // namespace EnemyAction