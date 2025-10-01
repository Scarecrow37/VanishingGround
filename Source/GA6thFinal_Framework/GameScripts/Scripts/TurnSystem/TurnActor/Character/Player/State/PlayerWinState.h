#pragma once
#include "Base/PlayerStateBase.h"

//플레이어가 전투 승리시 진입되는 상태입니다.
class PlayerWinState : public PlayerStateBase
{
public:
    PlayerWinState();
    virtual ~PlayerWinState() override;

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};