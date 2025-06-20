#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

/*
* 현재 턴인 엑터가 Player인지 확인하는 조건입니다.
*/
class PlayerActionCondition : public FSMCondition
{
public:
    PlayerActionCondition();
    virtual ~PlayerActionCondition() override;

protected:
    virtual void OnAwake() override;
    virtual void OnStart() override;
    virtual bool Evaluate() override;

    class TurnMode* _turnMode = nullptr;
};