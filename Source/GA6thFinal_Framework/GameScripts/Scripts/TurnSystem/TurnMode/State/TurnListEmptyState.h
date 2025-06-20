#pragma once
#include "Base/GetTurnModeBaseS.h"

/// <summary>
/// 턴 리스트가 비어있는지 확인하는 상태입니다.
/// </summary>
class TurnListEmptyState : public GetTurnModeBaseS
{
public:
    TurnListEmptyState();
    virtual ~TurnListEmptyState();

    // GetTurnModeBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};