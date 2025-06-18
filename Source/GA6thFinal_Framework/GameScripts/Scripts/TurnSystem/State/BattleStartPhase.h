#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class BattleStartPhase : public FSMState
{
public:
    BattleStartPhase();
    virtual ~BattleStartPhase() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};