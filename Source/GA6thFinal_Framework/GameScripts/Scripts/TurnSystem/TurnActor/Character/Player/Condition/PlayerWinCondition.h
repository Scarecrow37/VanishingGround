#pragma once
#include "Base/PlayerConditionBase.h"

//플레이어 승리 조건
class PlayerWinCondition : public PlayerConditionBase
{
public:
    PlayerWinCondition();
    virtual ~PlayerWinCondition() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
   
};