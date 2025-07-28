#pragma once
#include <EnemyAction/EnemyActionBase.h>

class EnemyActionFactory 
    : public FactoryConstructor<EnemyAction::ActionBase>
{

};
