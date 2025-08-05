#include "pchScripts.h"
#include "EnemyAction22013.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

namespace EnemyAction
{
    void Action22013::OnActionEnter() 
    {
        if (_animator)
        {
            _animator->BeginBuildOverrideAnimation();
            {
                _animator->ClearOverrideAnimations();
                bool result = _animator->PushOverrideAnimation("Attack1", true, [](const AnimationData& data) { return data.IsEnd(); });
                if (result)
                {
                    _animator->SetCurrentAnimationPopCallback([this]() { SetActionEnd(); });
                    _animator->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
                }
                else
                {
                    SetActionEnd();
                }
            }
            _animator->EndBuildOverrideAnimation();
        }
        else
        {
            SetActionEnd();
        }
    }
    void Action22013::OnActionUpdate() 
    {
    }
    void Action22013::OnActionExit() 
    {
    }
    void Action22013::OnAnimationEvent(const Timeline::EventContext* context) 
    {   
        const std::string& label = context->GetLabel();
        if ("Attack" == label)
        {
            ProcessBattle(30);
            if (_owner)
            {
                _owner->Dead();
            }
        }
    }
} // namespace EnemyAction