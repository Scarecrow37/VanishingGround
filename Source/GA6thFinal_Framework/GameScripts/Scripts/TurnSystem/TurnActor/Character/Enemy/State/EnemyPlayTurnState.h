#pragma once
#include "Base/EnemyStateBase.h"

/*
* 자신의 턴이 온 상태입니다.
*/
class EnemyPlayTurnState : public EnemyStateBase
{
public:



// EnemyStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
};