#pragma once
#include <EnemyAction/EnemyActionBase.h>

namespace EnemyAction
{
    /// <summary>
    /// 떨리는 중얼거림
    /// </summary>
    class Action22011 : public ActionBase
    {
    public:
        using ActionBase::ActionBase;
    private:
        void OnActionEnter() override;
        void OnActionUpdate() override;
        void OnActionExit() override;

        void OnAnimationEvent(const Timeline::EventContext* context) override;

    private:
    };
} // namespace EnemyAction