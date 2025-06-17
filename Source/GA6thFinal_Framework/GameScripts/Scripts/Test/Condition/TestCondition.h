#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"


class TestWorkCondition : public FSMCondition
{
public:
    TestWorkCondition();
    virtual ~TestWorkCondition() override;

    void OnAwake() override;
    void OnStart() override;

    bool Evaluate() override;

private:
    class TestComponent* testComponent;
};