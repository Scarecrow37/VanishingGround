#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class CombatStartCodition : public FSMCondition
{
public:
    CombatStartCodition();
    virtual ~CombatStartCodition() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};