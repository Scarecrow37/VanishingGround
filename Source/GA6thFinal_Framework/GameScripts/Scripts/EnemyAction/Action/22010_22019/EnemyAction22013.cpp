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
                const char* animKey = _owner->GetAnimationName(CharacterBase::ATTACK_3);
                _animator->ClearOverrideAnimations();
                _animator->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
                _animator->SetCurrentAnimationPopCallback([this]() { SetActionEnd(); });
                _animator->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            }
            _animator->EndBuildOverrideAnimation();
        }
    }
    void Action22013::OnActionUpdate() 
    {
    }
    void Action22013::OnActionExit() 
    {
        if (_owner)
        {
            _owner->Dead();
        }
    }
    void Action22013::OnAnimationEvent(const Timeline::EventContext* context) 
    {   
        const std::string& label = context->GetLabel();
        if ("Attack" == label)
        {
            ProcessBattle(30);
        }
    }
} // namespace EnemyAction