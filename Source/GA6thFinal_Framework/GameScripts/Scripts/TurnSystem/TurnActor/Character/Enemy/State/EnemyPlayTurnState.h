#pragma once
#include "Base/EnemyStateBase.h"
#include "../Enum/EnemyEnum.h"

class RoundInfoUIManager;
namespace EnemyAction
{
    class ActionBase;
}

/*
자신의 턴이 온 상태입니다.
*/
class EnemyPlayTurnState : public EnemyStateBase
{
    using Action = EnemyAction::ActionBase;

public:
    void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;

private:
    // EnemyStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    REFLECT_FIELDS_BEGIN(EnemyStateBase)
    REFLECT_FIELDS_END(EnemyPlayTurnState)

    std::weak_ptr<RoundInfoUIManager> _roundInfoUIManager;
    bool                              _isOnce = false;
};