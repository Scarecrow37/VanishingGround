#pragma once
#include "Base/EnemyStateBase.h"

/*
 * 적이 자신의 턴을 기다리는 상태입니다.
 */
class EnemyWaitTurnState : public EnemyStateBase
{
public:
    EnemyWaitTurnState();
    virtual ~EnemyWaitTurnState() override;
    bool IsMyTurn();

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

};