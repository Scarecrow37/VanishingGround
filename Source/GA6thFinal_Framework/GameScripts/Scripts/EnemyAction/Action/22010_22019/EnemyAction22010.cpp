#include "pchScripts.h"
#include "EnemyAction22010.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>

namespace EnemyAction
{
    void Action22010::OnActionEnter() 
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
        ProcessBattle(4);
    }

    void EnemyAction::Action22010::OnActionUpdate()
    {
    }

    void Action22010::OnActionExit() 
    {
    }
} // namespace EnemyAction