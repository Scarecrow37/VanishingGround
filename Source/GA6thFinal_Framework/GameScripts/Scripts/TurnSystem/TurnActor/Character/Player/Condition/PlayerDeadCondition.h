#pragma once
#include "Base/PlayerConditionBase.h"

/*
* 플레이어 사망 조건을 체크합니다.
*/
class PlayerDeadCondition : public PlayerConditionBase
{
public:
    PlayerDeadCondition();
    virtual ~PlayerDeadCondition() override;

    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};