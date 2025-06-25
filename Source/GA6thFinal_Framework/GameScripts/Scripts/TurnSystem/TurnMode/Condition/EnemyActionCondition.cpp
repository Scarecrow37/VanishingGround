#include "pchScripts.h"
#include "EnemyActionCondition.h"

REGISTER_CLASS(FSMConditionFactory, EnemyActionCondition)

EnemyActionCondition::EnemyActionCondition() 
{
}

EnemyActionCondition::~EnemyActionCondition() 
{
}

bool EnemyActionCondition::Evaluate()
{   
    return !PlayerActionCondition::Evaluate();
}
