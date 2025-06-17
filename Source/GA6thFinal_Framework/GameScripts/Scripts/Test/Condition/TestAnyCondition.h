#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class TestAnyCondition : public FSMCondition
{
public:
    TestAnyCondition();
    virtual ~TestAnyCondition() override;

    void OnAwake() override;
    void OnStart() override;

    bool Evaluate() override;
};