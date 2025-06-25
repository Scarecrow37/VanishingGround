#pragma once
#include <GameCore/FSM/FiniteStateMachine.h>

class Enemy;
class EnemyStateBase : public FSMState
{
public:
    EnemyStateBase();
    virtual ~EnemyStateBase() override;

protected:
    Enemy& GetEnemy();

private:
    Enemy* _enemy = nullptr;
};