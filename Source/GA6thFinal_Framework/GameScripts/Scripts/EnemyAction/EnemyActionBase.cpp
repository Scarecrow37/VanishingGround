#include "pchScripts.h"
#include "EnemyActionBase.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>

namespace EnemyAction
{
    ActionBase::ActionBase(Enemy* owner) 
        : _owner(owner)
    {
        if (_owner)
        {
            _animator = _owner->GetAnimationComponent();
        }
    }

    void ActionBase::RequireActionEnter()
    {
        _isActionEnd = false;
        OnActionEnter();
    }

    void ActionBase::RequireActionUpdate()
    {
        OnActionUpdate();
    }

    void ActionBase::RequireActionExit()
    {
        OnActionExit();
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
                //TurnMode::Battle()(*enemy, *player);
            }
        }
    }
} // namespace EnemyAction