#pragma once
#include <TurnSystem/TurnActor/Character/Enemy/Action/AnemyActionBase.h>

namespace EnemyAction
{
    /// <summary>
    /// 소멸의 찬가
    /// </summary>
    class Action22013 : public ActionBase
    {
    public:
        using ActionBase::ActionBase;
    private:
        void OnActionEnter() override;
        void OnActionUpdate() override;
        void OnActionExit() override;

    private:
    };
} // namespace EnemyAction