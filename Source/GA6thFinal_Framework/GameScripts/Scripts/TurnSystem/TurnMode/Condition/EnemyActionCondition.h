#pragma once
#include "PlayerActionCondition.h"  

class EnemyActionCondition : public PlayerActionCondition
{
public:
    EnemyActionCondition();
    virtual ~EnemyActionCondition() override;

protected:
    virtual bool Evaluate() override;
};