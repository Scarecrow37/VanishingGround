#include "pchScripts.h"
#include "EnemyAction22012.h"
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/Actions/Token/TokenApplyAction.h>
#include <Token/Object/Stun/StunToken.h>
namespace EnemyAction
{
    Action22012::Action22012(Enemy* owner) 
        : ActionBase(owner), _tokenAction(std::make_unique<TokenApplyAction>())
    {
        _tokenAction->TokenID    = TokenObject::Stun::ID;
        _tokenAction->TokenCount = 1;
    }
    Action22012::~Action22012() 
    {
    }
    void Action22012::OnActionEnter() 
    {
        bool result = false;
        if (_animator)
        {
            if (_animator->HasAnimationMappingKey("Attack0"))
            {
                _animator->BeginBuildOverrideAnimation();

                _animator->ClearOverrideAnimations();
                _animator->SetNextAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE | ANIMATION_FLAG_USE_BLEND);
                result = _animator->PushBackOverrideAnimation("Attack0");
                if (result)
                {
                    _animator->SetCurrentAnimationPopCondition([](const AnimationData& data) { return data.IsEnd(); }); // 애니메이션이 끝날 경우 Pop
                    _animator->SetCurrentAnimationPopCallback([this]() { SetActionEnd(); });
                }

                _animator->EndBuildOverrideAnimation();
            }
        }
        if (false == result)
        {
            SetActionEnd();
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
            TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
            if (turnMode)
            {
                turnMode->AddTurnAction(_tokenAction.get());
                ProcessBattle(1);
                _tokenAction->SetDestroy();
            }
        }
    }
} // namespace EnemyAction