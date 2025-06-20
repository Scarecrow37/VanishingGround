#pragma once
#include "Base/GetTurnModeBaseC.h"

class CheckTurnEmpty : public GetTurnModeBaseC
{
public:
    CheckTurnEmpty();
    virtual ~CheckTurnEmpty() override; 

    virtual void OnAwake() override;
    virtual void OnStart() override;
    virtual bool Evaluate() override;
};