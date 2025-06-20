#pragma once
#include "Base/GetTurnModeBase.h"

class RoundStartPhase : public GetTurnModeBase
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