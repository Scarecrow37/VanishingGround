#pragma once
#include <TurnSystem/TurnActor/Character/Enemy/Action/AnemyActionBase.h>

namespace EnemyAction
{
    /// <summary>
    /// 피의 의식
    /// </summary>
    class Action22004 : public ActionBase
    { 
    public:
        using ActionBase::ActionBase;
    private:
        void OnActionEnter() override;
        bool OnActionUpdate() override;
        void OnActionExit() override;

    private:
    };
} // namespace EnemyAction