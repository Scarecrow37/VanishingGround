#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class TesWorkState : public FSMState
{
public:
    TesWorkState();
    virtual ~TesWorkState() override;


// FSMState을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};