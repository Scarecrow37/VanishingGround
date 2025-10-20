#include "pchScripts.h"
#include "EnemyAction22010.h"
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/Actions/TokenApplyAction/TokenApplyAction.h>
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

    void Action22010::OnActionUpdate()
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
            TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
            if (turnMode)
            {
                turnMode->AddTurnAction(_tokenAction.get());
                ProcessBattle(5);
                _tokenAction->SetDestroy();
            }
        }
    }
} // namespace EnemyAction