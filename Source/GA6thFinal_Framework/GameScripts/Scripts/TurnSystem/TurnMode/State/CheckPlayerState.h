#pragma once
#include "Base/GetTurnModeBaseS.h"

class CheckPlayerState : public GetTurnModeBaseS
{
public:
    CheckPlayerState();
    virtual ~CheckPlayerState();

    // GetTurnModeBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};