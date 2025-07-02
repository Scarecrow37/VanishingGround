#pragma once
#include "Base/PlayerConditionBase.h"

/*
* 플레이어의 턴 종료 조건입니다.
*/
class PlayerExitCondition : public PlayerConditionBase
{
public:
    PlayerExitCondition();
    virtual ~PlayerExitCondition() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};