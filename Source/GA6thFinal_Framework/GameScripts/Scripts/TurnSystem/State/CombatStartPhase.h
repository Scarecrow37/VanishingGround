#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class CombatStartPhase : public FSMState
{
public:
    CombatStartPhase();
    virtual ~CombatStartPhase() override;

    bool IsEndPhase() { return _phaseEnd; }

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    bool _phaseEnd;
};