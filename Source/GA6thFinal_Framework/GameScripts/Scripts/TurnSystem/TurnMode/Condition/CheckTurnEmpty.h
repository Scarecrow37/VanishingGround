#pragma once
#include "Base/TurnModeConditionBase.h"

/*
* TurnList 항목이 비어있는지 확인하는 조건 입니다.
*/
class CheckTurnEmpty : public TurnModeConditionBase
{
public:
    CheckTurnEmpty();
    virtual ~CheckTurnEmpty() override; 

    virtual void OnAwake() override;
    virtual void OnStart() override;
    virtual bool Evaluate() override;
};