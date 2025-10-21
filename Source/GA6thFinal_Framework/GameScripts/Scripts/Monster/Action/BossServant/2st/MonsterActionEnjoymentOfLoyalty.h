#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    namespace Action
    {
        /// <summary>
        /// 충성의 향유
        /// </summary>
        class EnjoymentOfLoyalty : public Base
        {
            MONSTER_ACTION_DATA(210280)
        public:
            EnjoymentOfLoyalty();
            ~EnjoymentOfLoyalty() override;

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