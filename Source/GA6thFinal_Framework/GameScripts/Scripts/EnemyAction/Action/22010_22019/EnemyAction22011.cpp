#include "pchScripts.h"
#include "EnemyAction22011.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

namespace EnemyAction
{
    void Action22011::OnActionEnter() 
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
    void EnemyAction::Action22011::OnActionUpdate() 
    {
    }
    void Action22011::OnActionExit() 
    {
    }
    void Action22011::OnAnimationEvent(const Timeline::EventContext* context) 
    {
        const std::string& label = context->GetLabel();
        if ("Attack_1" == label || "Attack_2" == label)
        {
            ProcessBattle(4);
        }
    }
} // namespace EnemyAction