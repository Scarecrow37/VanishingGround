#pragma once
#include "Base/PlayerStateBase.h"

/*
* 플레이어의 턴이 시작된 상태입니다.
*/
class PlayerPlayTurnState : public PlayerStateBase, public InputReceiver
{
public:
    //플레이어의 입력 상태
    enum class InputState
    {
        //자신의 턴이 아닌상태
        NONE,
        //행동 선택 상태
        ACTION_SELECTION,
        //QTE 선택 상태
        QUICK_TIME_EVENT,
    };

    PlayerPlayTurnState();
    virtual ~PlayerPlayTurnState() override;
protected:
    // PlayerStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    void TestAttack(Enemy* dest, int damage);

private:
    void UpdateAttackButtonHeld(float dt);
    void PressedAButton(const Input::Controller& controller);
    void ReleasedAButton(const Input::Controller& controller);
    
    void UpdateActionSelectionUI(float dt);
    void UpdateQuickTimeEventUI(float dt);

private:
    bool       _setImguiPosCenter;
    InputState _inputState;
    bool       _isDownAttackButton;
    float      _attackButtonHeldTime;
    float      _attackButtonHeldWaitTime;
    int        _attackRemaining;
    bool       _isAttacking = false;
};