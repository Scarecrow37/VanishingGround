#pragma once
#include <TurnSystem/TurnActor/Character/Enemy/Action/AnemyActionBase.h>

namespace EnemyAction
{
    /// <summary>
    /// 기습
    /// </summary>
    class Action22001 : public ActionBase
    { public:
        using ActionBase::ActionBase;
    private:
        void OnActionEnter() override;
        bool OnActionUpdate() override;
        void OnActionExit() override;

    private:
    };
} // namespace EnemyAction