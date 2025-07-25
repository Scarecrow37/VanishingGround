#include "pchScripts.h"
#include "EnemyDeadCondition.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMConditionFactory, EnemyDeadCondition)

void EnemyDeadCondition::OnAwake() {}

void EnemyDeadCondition::OnStart() {}

bool EnemyDeadCondition::Evaluate()
{
    TurnActor::STATE state = GetEnemy().GetActorState();
    return state == TurnActor::STATE::Dead;
}
