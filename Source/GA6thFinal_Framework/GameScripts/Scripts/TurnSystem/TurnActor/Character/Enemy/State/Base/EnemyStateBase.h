#pragma once
#include <GameCore/FSM/FiniteStateMachine.h>

class Enemy;
class EnemyStateBase : public FSMState
{
public:
    EnemyStateBase();
    virtual ~EnemyStateBase() override;

public:
    virtual void OnNotifiedAnimationEvent(const Timeline::EventContext* context) {}

protected:
    Enemy& GetEnemy();
    REFLECT_FIELDS_BEGIN(FSMState)
    REFLECT_FIELDS_END(EnemyStateBase)

private:
    Enemy* _enemy = nullptr;
  
};