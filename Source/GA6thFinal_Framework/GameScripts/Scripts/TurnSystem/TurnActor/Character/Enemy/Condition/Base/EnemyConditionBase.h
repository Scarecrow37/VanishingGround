#pragma once
#include <GameCore/FSM/FiniteStateMachine.h>

class Enemy;
class EnemyConditionBase : public FSMCondition
{
public:
    EnemyConditionBase();
    virtual ~EnemyConditionBase() override;

protected:
    Enemy& GetEnemy();

private:
    Enemy* _enemy = nullptr;
};