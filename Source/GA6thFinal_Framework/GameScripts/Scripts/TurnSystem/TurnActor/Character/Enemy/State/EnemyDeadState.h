#pragma once
#include "Base/EnemyStateBase.h"

class EnemyDeadState : public EnemyStateBase
{
public:
// EnemyStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;
};