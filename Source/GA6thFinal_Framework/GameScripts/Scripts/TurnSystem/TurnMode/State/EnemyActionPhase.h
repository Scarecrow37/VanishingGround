#pragma once
#include "Base/TurnModeStateBase.h"

/*
* 몬스터 행동 페이즈용 State 입니다.
* 몬스터를 행동 모드로 전환합니다.
*/
class EnemyActionPhase : public TurnModeStateBase
{
public:
    EnemyActionPhase();
    virtual ~EnemyActionPhase() override;

    // FSMState을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    bool WaitPhase = true;
};