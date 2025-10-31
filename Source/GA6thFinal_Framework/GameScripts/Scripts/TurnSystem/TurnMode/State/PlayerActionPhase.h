#pragma once
#include "Base/TurnModeStateBase.h"

/*
* 플레이어 행동 페이즈 State 입니다.
* 플레이어를 행동 모드로 전환해야합니다.
*/
class PlayerActionPhase : public TurnModeStateBase
{
public:
    PlayerActionPhase();
    virtual ~PlayerActionPhase() override;

// FSMState을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

public:
    bool WaitPhase = true;
};