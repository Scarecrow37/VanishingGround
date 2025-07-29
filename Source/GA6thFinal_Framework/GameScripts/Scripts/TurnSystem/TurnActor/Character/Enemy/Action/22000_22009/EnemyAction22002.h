#pragma once
#include <TurnSystem/TurnActor/Character/Enemy/Action/AnemyActionBase.h>

namespace EnemyAction
{
    /// <summary>
    /// 확인 사살
    /// </summary>
    class Action22002 : public ActionBase
    { public:
        using ActionBase::ActionBase;
    private:
        void OnActionEnter() override;
        void OnActionUpdate() override;
        void OnActionExit() override;

    private:
    };
} // namespace EnemyAction