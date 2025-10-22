#pragma once

#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    namespace Action
    {
        /// <summary>
        /// 연기 베기
        /// </summary>
        class SmokeSlash : public Base
        {
            MONSTER_ACTION_DATA(210203)
        public:
            SmokeSlash();
            ~SmokeSlash() override;

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
