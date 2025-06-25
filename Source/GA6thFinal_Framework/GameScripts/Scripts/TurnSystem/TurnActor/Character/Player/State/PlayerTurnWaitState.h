#pragma once
#include "Base/PlayerStateBase.h"

/*
* 플레이어의 턴을 기다리는 상태입니다.
*/
class PlayerTurnWaitState : public PlayerStateBase
{
public:
    PlayerTurnWaitState();
    virtual ~PlayerTurnWaitState() override;
    bool IsMyTurn();
protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};