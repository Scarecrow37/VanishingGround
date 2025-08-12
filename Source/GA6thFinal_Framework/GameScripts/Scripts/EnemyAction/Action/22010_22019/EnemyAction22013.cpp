#include "pchScripts.h"
#include "EnemyAction22013.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnActor/Character/Enemy/State/EnemyDeadState.h>

namespace EnemyAction
{
    void Action22013::OnActionEnter() 
    {
        if (_animator)
        {
            _animator->BeginBuildOverrideAnimation();
            {
                if (_animator->HasAnimationMappingKey("Attack1"))
                {
                    _animator->ClearOverrideAnimations();
                    _animator->SetNextAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE | ANIMATION_FLAG_USE_BLEND);
                    _animator->ChangeMainAnimation("Attack1");
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
                _owner->TakeDamage(_owner->HP, false);
                EnemyDeadState* deadState = _owner->GetFSMStates().Dead;
                if (deadState)
                {
                    deadState->SetDontChangeAnimation(true);
                }
            }
            SetActionEnd();
        }
    }
} // namespace EnemyAction