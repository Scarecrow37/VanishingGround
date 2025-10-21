#pragma once

#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    namespace Action
    {
        /// <summary>
        /// 피의 의식
        /// </summary>
        class BloodRitual : public Base
        {
            MONSTER_ACTION_DATA(210204)
        public:
            BloodRitual();
            ~BloodRitual() override;

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
