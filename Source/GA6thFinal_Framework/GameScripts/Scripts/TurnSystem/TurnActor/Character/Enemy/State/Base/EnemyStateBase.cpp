#include "pchScripts.h"
#include "EnemyStateBase.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REFLECT_FUNCTION(EnemyStateBase)

EnemyStateBase::EnemyStateBase() {}
EnemyStateBase::~EnemyStateBase() {}

Enemy& EnemyStateBase::GetEnemy()
{
    if (nullptr == _enemy)
    {
        _enemy = GetFSM().GetComponent<Enemy>();
    }
    return *_enemy;
}
