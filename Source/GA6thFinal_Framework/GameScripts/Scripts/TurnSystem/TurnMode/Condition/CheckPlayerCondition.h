#pragma once
#include "Base/GetTurnModeBaseC.h"

class CheckPlayerCondition : public GetTurnModeBaseC
{
public:
    CheckPlayerCondition();
    virtual ~CheckPlayerCondition();

    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};