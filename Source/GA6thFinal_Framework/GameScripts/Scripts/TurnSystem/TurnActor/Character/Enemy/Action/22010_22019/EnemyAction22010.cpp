#include "pchScripts.h"
#include "EnemyAction22010.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>

namespace EnemyAction
{
    void Action22010::OnActionEnter() 
    {
        if (_owner)
        {
            _renderer = _owner->GetSkeletalMeshRenderer();
            if (_renderer)
            {
                _renderer->SetCurrentAnimation("Idle");
            }
        }
    }

    bool Action22010::OnActionUpdate()
    {
        if (_owner && _renderer)
        {
            SkeletalMeshRenderer* renderer = _owner->GetSkeletalMeshRenderer();
            if (renderer)
            {
                bool isAnimationEnd = renderer->IsAnimationEnd();
                if (true == isAnimationEnd)
                {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    void Action22010::OnActionExit() 
    {
    }
} // namespace EnemyAction