#include "pchScripts.h"
#include "EnemyAction22012.h"
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/Token/TokenApplyAction.h>
#include <Token/Object/Stun/StunToken.h>
namespace EnemyAction
{
    Action22012::Action22012(Enemy* owner) 
        : ActionBase(owner), _tokenAction(new TokenApplyAction)
    {
        _tokenAction->TokenID    = TokenObject::Stun::ID;
        _tokenAction->TokenCount = 1;
    }
    Action22012::~Action22012() 
    {
        if (_tokenAction)
        {
            delete _tokenAction;
            _tokenAction = nullptr;
        }
    }
    void Action22012::OnActionEnter() 
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
    void EnemyAction::Action22012::OnActionUpdate() 
    {
    }
    void Action22012::OnActionExit() 
    {
    }
    void Action22012::OnAnimationEvent(const Timeline::EventContext* context)
    {
        const std::string& label = context->GetLabel();
        if ("Attack_1" == label || "Attack_2" == label)
        {
            TurnMode* turnMode = TurnMode::GetInstance();
            if (turnMode)
            {
                turnMode->AddTurnAction(_tokenAction);
                ProcessBattle(1);
                _tokenAction->SetDestroy();
            }
        }
    }
} // namespace EnemyAction