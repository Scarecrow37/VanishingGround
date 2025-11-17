#pragma once
#include "Base/TurnModeConditionBase.h"

/*
* 현재 TurnActor의 상태가 Wait 상태로 돌아갔는지 확인하는 조건입니다.
*/
class CheckTurnEndCondition : public TurnModeConditionBase
{
public:
    CheckTurnEndCondition();
    virtual ~CheckTurnEndCondition();

    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;

    bool IsTurnEnd = false;
};