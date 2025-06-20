#pragma once
#include "PlayerActionCondition.h"  

/*
* 현재 턴인 Actor가 Enemy인지 확인하는 조건입니다.,
*/
class EnemyActionCondition : public PlayerActionCondition
{
public:
    EnemyActionCondition();
    virtual ~EnemyActionCondition() override;

protected:
    virtual bool Evaluate() override;
};