#include "EnemyActionCondition.h"

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
