#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 후려치기
    /// </summary>
    namespace Action
    {
        class SmashStrike : public Base
        {
            MONSTER_ACTION_DATA(210220)
        public:
            SmashStrike();
            ~SmashStrike() override;

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
