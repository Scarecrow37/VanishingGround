#pragma once
#include <EnemyAction/EnemyActionBase.h>

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
        void OnActionUpdate() override;
        void OnActionExit() override;

    private:
    };
} // namespace EnemyAction