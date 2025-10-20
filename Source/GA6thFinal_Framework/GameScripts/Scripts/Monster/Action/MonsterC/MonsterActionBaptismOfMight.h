#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 힘의 세례
    /// </summary>
    namespace Action
    {
        class BaptismOfMight : public Base
        {
            MONSTER_ACTION_DATA(210222)
        public:
            BaptismOfMight();
            ~BaptismOfMight() override;

        private:
            void OnActionEnter() override;
            void OnActionUpdate() override;
            void OnActionExit() override;
            void OnActionReset() override;
            void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;

        private:
            void Behavior();
        };
    } // namespace Action
} // namespace Monster

class MonsterActionBaptismOfMight
{
};
