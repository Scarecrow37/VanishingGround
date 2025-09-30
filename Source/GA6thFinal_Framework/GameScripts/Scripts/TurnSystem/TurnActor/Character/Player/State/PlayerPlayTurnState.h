#pragma once
#include "Base/PlayerStateBase.h"
#include <BattleSystem/Battle.h>
#include <QTE/Result/QTEResult.h>
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

    PlayerPlayTurnState();
    virtual ~PlayerPlayTurnState() override;
protected:
    // PlayerStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    void OnQTEFinish();

    void PressedButtonA(const Input::Controller& controller);
    void ReleasedButtonA(const Input::Controller& controller);

private:
    void UpdateAttackButtonHeld(float dt);
    
    void UpdateActionSelectionUI(float dt);
    void UpdateQuickTimeEventUI(float dt);
    void UpdateAttackEventUI(float dt);

    void SetAttackReady();
    void SetAttack();
    void SetAttackEnd();

    void BattleOnHitEvent(const QTE::NoteResult& result);

    Battle::EnemyTargetFlag_ GetAttackTargetFromButton(unsigned int button) const;

private:
    InputState _inputState;
    int        _attackRemaining;        // 공격 남은 횟수
    bool       _isDownAButton;          // QTE 시작 버튼 (패드)
    bool       _isDownAKey;             // QTE 시작 버튼 (키보드)
    float      _attackButtonHeldTime;
    float      _attackButtonHeldWaitTime;

    //== Debug == //
    bool       _showDebugUI = false;
    
};
