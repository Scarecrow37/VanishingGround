#pragma once
#include "Base/TurnModeConditionBase.h"

class GameClearCondition : public TurnModeConditionBase
{
public:
    GameClearCondition();
    virtual ~GameClearCondition() override;

protected:
    void OnAwake() override;
    bool Evaluate() override;

};