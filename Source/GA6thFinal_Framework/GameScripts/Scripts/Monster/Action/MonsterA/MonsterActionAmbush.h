#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    namespace Action
    {
        /// <summary>
        /// 기습
        /// </summary>
        class Ambush : public Base
        {
            MONSTER_ACTION_DATA(210201)
        public:
            Ambush();
            ~Ambush() override;

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
