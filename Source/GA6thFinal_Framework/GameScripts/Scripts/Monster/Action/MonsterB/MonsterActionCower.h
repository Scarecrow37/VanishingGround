#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 웅크리기
    /// </summary>
    namespace Action
    {
        class Cower : public Base
        {
            MONSTER_ACTION_DATA(210214)
        public:
            using Base::Base;

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