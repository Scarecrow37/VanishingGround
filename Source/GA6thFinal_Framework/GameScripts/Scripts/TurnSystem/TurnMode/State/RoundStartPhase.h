#pragma once
#include "Base/GetTurnModeBaseS.h"

class RoundStartPhase : public GetTurnModeBaseS
{
public:
    RoundStartPhase();
    virtual ~RoundStartPhase() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};