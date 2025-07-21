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
            _owner->SetAnimation(CharacterBase::ATTACK_1, false);
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
        // owner와 renderer 둘 중 하나라도 없다면 스킵
        return true;
    }

    void Action22010::OnActionExit() 
    {
    }
} // namespace EnemyAction