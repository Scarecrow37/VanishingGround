#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class TestState : public FSMState
{
public:
    TestState();
    virtual ~TestState() override;
};