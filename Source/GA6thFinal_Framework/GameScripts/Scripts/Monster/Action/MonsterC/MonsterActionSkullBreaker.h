#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 두개골 부수기
    /// </summary>
    namespace Action
    {
        class SkullBreaker : public Base
        {
            MONSTER_ACTION_DATA(210221)
        public:
            SkullBreaker();
            ~SkullBreaker() override;

        private:
            void OnActionEnter() override;
            void OnActionUpdate() override;
            void OnActionExit() override;
            void OnActionReset() override;
            void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;

        private:
            void Attack();
        };
    } // namespace Action
} // namespace Monster