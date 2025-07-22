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

    void ActionBase::SetMainAnimation(int animType, bool loop, bool blend) 
    {
        if (_owner && _renderer)
        {
            _owner->SetMainAnimation((CharacterBase::AnimationType)animType, loop, blend);
        }
    }

    void ActionBase::PushOverrideAnimation(int animType, bool loop, bool blend,
                                   std::function<bool(const AnimationData&)> popCondition)
    {
        if (_owner && _renderer)
        {
            _owner->PushOverrideAnimation((CharacterBase::AnimationType)animType, loop, blend, popCondition);
        }
    }

    void ActionBase::PopOverrideAnimation(bool blend)
    {
        if (_owner && _renderer)
        {
            _owner->PopOverrideAnimation(blend);
        }
    }

    bool ActionBase::IsAnimationEnd()
    {
        if (_owner && _renderer)
        {
            SkeletalMeshRenderer* renderer = _owner->GetSkeletalMeshRenderer();
            if (renderer)
            {
                if (true == renderer->GetLastAnimationData().IsEnd)
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