#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"


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