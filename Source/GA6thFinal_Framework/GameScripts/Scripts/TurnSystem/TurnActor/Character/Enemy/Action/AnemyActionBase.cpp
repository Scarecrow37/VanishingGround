#include "pchScripts.h"
#include "AnemyActionBase.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Animation/AnimationComponent.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <BattleSystem/Battle.h>
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
                Battle()(*enemy, *player);
            }
        }
    }
} // namespace EnemyAction