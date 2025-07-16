#pragma once
#include "Base/PlayerStateBase.h"

/*
* 플레이어의 턴이 시작된 상태입니다.
*/
class PlayerPlayTurnState : public PlayerStateBase
{
public:
    PlayerPlayTurnState();
    virtual ~PlayerPlayTurnState() override;
protected:
    // PlayerStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    bool _isStart = false;
};