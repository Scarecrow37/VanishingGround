#pragma once
#include <EnemyAction/EnemyActionBase.h>

class TokenApplyAction;

namespace EnemyAction
{
    /// <summary>
    /// 두려움의 중얼거림
    /// </summary>
    class Action22010 : public ActionBase
    {
    public:
        Action22010(Enemy* owner);
        virtual ~Action22010();
    private:
        void OnActionEnter() override;
        void OnActionUpdate() override;
        void OnActionExit() override;

        void OnAnimationEvent(const Timeline::EventContext* context) override;

    private:

        TokenApplyAction* tokenAction;
    };
} // namespace EnemyAction