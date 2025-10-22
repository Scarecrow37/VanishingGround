#pragma once
#include <EnemyAction/EnemyActionBase.h>

class AttackTokenApplyAction;

namespace EnemyAction
{
    /// <summary>
    /// 절망
    /// </summary>
    class Action22012 : public ActionBase
    {
    public:
        Action22012(Enemy* owner);
        virtual ~Action22012();
    private:
        void OnActionEnter() override;
        void OnActionUpdate() override;
        void OnActionExit() override;

        void OnAnimationEvent(const Timeline::EventContext* context) override;

    private:
        // 기절 부여용 액션
        std::unique_ptr<AttackTokenApplyAction> _tokenAction;
    };
} // namespace EnemyAction