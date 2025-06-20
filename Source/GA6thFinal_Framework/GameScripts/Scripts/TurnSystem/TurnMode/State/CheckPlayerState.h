#pragma once
#include "Base/GetTurnModeBaseS.h"

/*
* 현재 선택된 CurrActor가 Player인지 확인하는 상태입니다.
*/
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