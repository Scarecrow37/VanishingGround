#pragma once
#include "Base/PlayerConditionBase.h"

/*
* 플레이어의 턴 유무를 확인하는 조건입니다.
*/
class PlayerStartCondition : public PlayerConditionBase
{
public:
    PlayerStartCondition();
    virtual ~PlayerStartCondition() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};