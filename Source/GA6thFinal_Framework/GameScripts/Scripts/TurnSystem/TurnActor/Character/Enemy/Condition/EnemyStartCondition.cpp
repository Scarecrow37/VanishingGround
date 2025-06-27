#include "pchScripts.h"
#include "EnemyStartCondition.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMConditionFactory, EnemyStartCondition)

void EnemyStartCondition::OnAwake() 
{
}

void EnemyStartCondition::OnStart() 
{
}

bool EnemyStartCondition::Evaluate()
{
    return GetEnemy().GetActorState() == TurnActor::STATE::Play;
}
