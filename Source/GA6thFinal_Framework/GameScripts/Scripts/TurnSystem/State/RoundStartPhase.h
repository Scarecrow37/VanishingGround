#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class RoundStartPhase : public FSMState
{
public:
    RoundStartPhase();
    virtual ~RoundStartPhase() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};