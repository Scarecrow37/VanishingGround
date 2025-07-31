#include "pchScripts.h"
#include "EnemyAction22011.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

namespace EnemyAction
{
    void Action22011::OnActionEnter() 
    {
        if (_animator)
        {
            _animator->BeginBuildOverrideAnimation();
            {
                const char* animKey = _owner->GetAnimationName(CharacterBase::ATTACK_1);
                _animator->ClearOverrideAnimations();
                _animator->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
                _animator->SetCurrentAnimationPopCallback([this]() { SetActionEnd(); });
                _animator->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            }
            _animator->EndBuildOverrideAnimation();
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
        if ("Attack" == label)
        {
            ProcessBattle(4);
        }
    }
} // namespace EnemyAction