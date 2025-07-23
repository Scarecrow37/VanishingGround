#include "pchScripts.h"
#include "EnemyAction22011.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>

namespace EnemyAction
{
    void Action22011::OnActionEnter() 
    {
        if (_renderer)
        {
            _renderer->BeginBuildOverrideAnimation();
            {
                const char*   animKey = _owner->GetAnimationName(CharacterBase::ATTACK_1);
                _renderer->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd; });
                _renderer->SetCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            }
            _renderer->EndBuildOverrideAnimation();
        }
        ProcessBattle();
    }
    bool Action22011::OnActionUpdate() 
    {
        if (_renderer)
        {
            return 0 == _renderer->GetOverrideAnimationCount();
        }
        return true;
    }
    void Action22011::OnActionExit() 
    {
    }
} // namespace EnemyAction