#include "pchScripts.h"
#include "AnemyActionBase.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>

namespace EnemyAction
{
    ActionBase::ActionBase(Enemy* owner) 
        : _owner(owner)
    {
        if (_owner)
        {
            _renderer = _owner->GetSkeletalMeshRenderer();
        }
    }

    void ActionBase::SetAnimation(int animType, bool loop, bool blend) 
    {
        if (_owner && _renderer)
        {
            _owner->SetAnimation((CharacterBase::AnimationType)animType, loop, blend);
        }
    }

    bool ActionBase::isAnimationEnd()
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
} // namespace EnemyAction