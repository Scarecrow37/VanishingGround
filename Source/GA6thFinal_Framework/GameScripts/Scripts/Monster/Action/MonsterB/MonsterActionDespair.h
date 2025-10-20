#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 절망
    /// </summary>
    namespace Action
    {
        class Despair : public Base
        {
            MONSTER_ACTION_DATA(210212)
        public:
            using Base::Base;

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