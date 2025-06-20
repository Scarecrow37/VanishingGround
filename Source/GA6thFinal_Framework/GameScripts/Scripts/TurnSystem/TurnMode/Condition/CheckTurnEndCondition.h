#pragma once
#include "Base/GetTurnModeBaseC.h"

class CheckTurnEndCondition : public GetTurnModeBaseC
{
public:
    CheckTurnEndCondition();
    virtual ~CheckTurnEndCondition();

    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};