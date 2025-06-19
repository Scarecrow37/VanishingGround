#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class CombatStartPhase;
class RoundStartCondition : public FSMCondition
{
public:
    RoundStartCondition();
    virtual ~RoundStartCondition() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;

private:
    CombatStartPhase* _combatStartPhase;
};