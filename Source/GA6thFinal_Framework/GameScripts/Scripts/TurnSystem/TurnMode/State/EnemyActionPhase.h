#pragma once
#include "Base/GetTurnModeBaseS.h"

class EnemyActionPhase : public GetTurnModeBaseS
{
public:
    EnemyActionPhase();
    virtual ~EnemyActionPhase() override;

    // FSMState을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};