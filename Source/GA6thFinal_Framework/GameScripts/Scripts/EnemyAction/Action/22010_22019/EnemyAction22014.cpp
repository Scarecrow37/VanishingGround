#include "pchScripts.h"
#include "EnemyAction22014.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

namespace EnemyAction
{
    void Action22014::OnActionEnter() 
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
    void Action22014::OnActionUpdate() 
    {
    }
    void Action22014::OnActionExit() 
    {
    }
    void Action22014::OnAnimationEvent(const Timeline::EventContext* context) 
    {
    }
} // namespace EnemyAction