#include "pchScripts.h"
#include "AnemyActionBase.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>
#include <TurnSystem/TurnMode/TurnMode.h>

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

    void ActionBase::ProcessBattle()
    {
        TurnMode* turnMode = TurnMode::GetInstance();
        if (turnMode)
        {
            Enemy*  enemy  = _owner;
            Player* player = turnMode->GetPlayer();
            if (enemy && player)
            {
                TurnMode::Battle()(*enemy, *player);
            }
        }
    }

    bool ActionBase::IsAnimationEnd()
    {
        if (_owner && _renderer)
        {
            SkeletalMeshRenderer* renderer = _owner->GetSkeletalMeshRenderer();
            if (renderer)
            {
                if (true == renderer->GetLastAnimationData().IsEnd())
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