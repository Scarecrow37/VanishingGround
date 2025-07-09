#pragma once
#include "Base/TurnModeConditionBase.h"

class GameOverCondition : public TurnModeConditionBase
{
public:
    GameOverCondition();
    virtual ~GameOverCondition() override;

protected:
    void OnAwake() override;
    bool Evaluate() override;

};