#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

/// <summary>
/// 항상 전이시키는 Condition 객체
/// </summary>
class AlwaysTransitionCondition : public FSMCondition
{
public:
    // FSMCondition을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};