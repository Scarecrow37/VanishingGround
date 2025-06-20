#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

/// <summary>
/// TurnMode를 Start에서 가져와주는 베이스 클래스입니다.
/// </summary>
class GetTurnModeBaseC : public FSMCondition
{
public:
    GetTurnModeBaseC();
    virtual ~GetTurnModeBaseC() override;

protected:
    void OnStart() override;

protected:
    class TurnMode* _turnMode = nullptr;
};