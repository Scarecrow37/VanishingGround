#include "pchScripts.h"
#include "EnemyAction22014.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

namespace EnemyAction
{
    void Action22014::OnActionEnter() 
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