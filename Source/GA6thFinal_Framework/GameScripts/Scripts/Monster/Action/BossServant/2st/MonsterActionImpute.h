#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    namespace Action
    {
        /// <summary>
        /// 전가
        /// </summary>
        class Impute : public Base
        {
            MONSTER_ACTION_DATA(210281)
        public:
            Impute();
            ~Impute() override;

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
