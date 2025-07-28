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
                const char* animKey = _owner->GetAnimationName(CharacterBase::ATTACK_3);
                _animator->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
                _animator->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            }
            _animator->EndBuildOverrideAnimation();
        }
        ProcessBattle();
    }
    bool Action22014::OnActionUpdate() 
    {
        if (_animator)
        {
            return 0 == _animator->GetOverrideAnimationCount();
        }
        return true;
    }
    void Action22014::OnActionExit() 
    {
    }
} // namespace EnemyAction