#pragma once
#include "Base/PlayerStateBase.h"

class Enemy;

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
        //QTE 연출 상태
        ATTACK_EVENT
    };

    //플레이어가 공격할 적 위치
    enum class AttackTarget
    {
        LEFT = 0,
        MIDDLE = 1,
        RIGHT = 2
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

    bool CheckAttackEnd();

private:
    void UpdateAttackButtonHeld(float dt);
    void PressedButtonA(const Input::Controller& controller);
    void ReleasedButtonA(const Input::Controller& controller);
    
    void PressedButtonX(const Input::Controller& controller);
    void ReleasedButtonX(const Input::Controller& controller);

    void PressedButtonY(const Input::Controller& controller);
    void ReleasedButtonY(const Input::Controller& controller);

    void PressedButtonB(const Input::Controller& controller);
    void ReleasedButtonB(const Input::Controller& controller);
    
    void UpdateActionSelectionUI(float dt);
    void UpdateQuickTimeEventUI(float dt);
    void UpdateAttackEventUI(float dt);

    bool IsAttackable() const;
    void PushAttackTarget(AttackTarget target);

private:
    bool       _setImguiPosCenter;
    InputState _inputState;
    bool       _isDownAttackButton;
    float      _attackButtonHeldTime;
    float      _attackButtonHeldWaitTime;
    int        _attackRemaining;
    std::vector<AttackTarget> _attackTargets;
};