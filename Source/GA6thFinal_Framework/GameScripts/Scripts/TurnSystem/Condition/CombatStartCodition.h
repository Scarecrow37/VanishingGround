#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class CombatStartPhase;
class CombatStartCodition : public FSMCondition
{
public:
    CombatStartCodition();
    virtual ~CombatStartCodition() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;

private:
    CombatStartPhase* _battleStartPhase;
};