#include "pchScripts.h"
#include "EnemyConditionBase.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

EnemyConditionBase::EnemyConditionBase() 
{

}

EnemyConditionBase::~EnemyConditionBase() 
{

}

Enemy& EnemyConditionBase::GetEnemy()
{
    if (nullptr == _enemy)
    {
        _enemy = GetFSM().GetComponent<Enemy>();
    }
    return *_enemy;
}
