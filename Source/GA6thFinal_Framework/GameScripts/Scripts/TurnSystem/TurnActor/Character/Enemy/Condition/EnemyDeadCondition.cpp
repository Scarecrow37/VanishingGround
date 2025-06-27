#include "pchScripts.h"
#include "EnemyDeadCondition.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

void EnemyDeadCondition::OnAwake() {}

void EnemyDeadCondition::OnStart() {}

bool EnemyDeadCondition::Evaluate()
{
    int  hp = GetEnemy().HP;
    bool isDead = 0 >= hp;
    return isDead;
}
