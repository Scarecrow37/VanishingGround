#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class BattleStartPhase : public FSMState
{
public:
    BattleStartPhase();
    virtual ~BattleStartPhase() override;

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