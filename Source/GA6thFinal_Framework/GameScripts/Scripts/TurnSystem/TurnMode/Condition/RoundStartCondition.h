#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"
class CombatStartPhase;

/*
* CombatStartPhase 종료 여부를 확인하는 조건입니다.
*/
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