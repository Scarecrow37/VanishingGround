#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class TestMoveCodition : public FSMCondition
{
public:
    TestMoveCodition();
    virtual ~TestMoveCodition() override;

    void OnAwake() override;
    void OnStart() override;

    bool Evaluate() override;

private:
    class TestComponent* testComponent = nullptr;
};