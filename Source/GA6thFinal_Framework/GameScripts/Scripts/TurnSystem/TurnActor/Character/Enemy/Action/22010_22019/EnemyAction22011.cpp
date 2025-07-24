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
                _animator->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
                _animator->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            }
            _animator->EndBuildOverrideAnimation();
        }
        ProcessBattle();
    }
    bool Action22011::OnActionUpdate() 
    {
        if (_animator)
        {
            return 0 == _animator->GetOverrideAnimationCount();
        }
        return true;
    }
    void Action22011::OnActionExit() 
    {
    }
} // namespace EnemyAction