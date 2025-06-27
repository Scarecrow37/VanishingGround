#include "pchScripts.h"
#include "EnemyEndCondition.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

void EnemyEndCondition::OnAwake() {}

void EnemyEndCondition::OnStart() {}

bool EnemyEndCondition::Evaluate()
{
    return GetEnemy().GetActorState() == TurnActor::STATE::Wait;
}
