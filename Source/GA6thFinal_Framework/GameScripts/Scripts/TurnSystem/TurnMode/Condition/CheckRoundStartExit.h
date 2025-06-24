#pragma once
#include "Base/TurnModeConditionBase.h"

/*
* 라운드 시작 연출이 종료됬는지 확인합니다.
*/
class CheckRoundStartExit : public TurnModeConditionBase
{
public:
    CheckRoundStartExit();
    virtual ~CheckRoundStartExit();

    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};