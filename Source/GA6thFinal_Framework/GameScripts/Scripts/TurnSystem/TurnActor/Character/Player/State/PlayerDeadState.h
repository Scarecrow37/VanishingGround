#pragma once
#include "Base/PlayerStateBase.h"

/*
* 플레이어가 사망하면 진입하는 상태입니다.
*/
class PlayerDeadState : public PlayerStateBase
{
public:
    PlayerDeadState();
    virtual ~PlayerDeadState() override;


// PlayerStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    class OverlayPanel*           _vanishedOverlay;
    class SpriteAnimationElement* _vanishedAnimation;
};