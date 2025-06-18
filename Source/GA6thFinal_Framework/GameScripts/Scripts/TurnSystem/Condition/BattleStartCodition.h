#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

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
    bool _isBattleStart;
};