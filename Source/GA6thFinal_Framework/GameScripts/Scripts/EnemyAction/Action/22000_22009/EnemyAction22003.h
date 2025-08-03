#pragma once
#include <EnemyAction/EnemyActionBase.h>

namespace EnemyAction
{
    /// <summary>
    /// 찢어 발기기
    /// </summary>
    class Action22003 : public ActionBase
    { public:
        using ActionBase::ActionBase;
    private:
        void OnActionEnter() override;
        void OnActionUpdate() override;
        void OnActionExit() override;

    private:
    };
} // namespace EnemyAction