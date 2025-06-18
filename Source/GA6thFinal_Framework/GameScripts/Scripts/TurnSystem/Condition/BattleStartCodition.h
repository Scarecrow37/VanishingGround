#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class BattleStartPhase;
class BattleStartCodition : public FSMCondition
{
public:
    BattleStartCodition();
    virtual ~BattleStartCodition() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;

private:
    BattleStartPhase* _battleStartPhase;
};