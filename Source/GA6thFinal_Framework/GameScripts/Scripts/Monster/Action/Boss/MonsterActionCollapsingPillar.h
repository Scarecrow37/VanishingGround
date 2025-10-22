#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 무너지는 기둥
    /// </summary>
    namespace Action
    {
        class CollapsingPillar : public Base
        {
            MONSTER_ACTION_DATA(210260)
        public:
            CollapsingPillar();
            ~CollapsingPillar() override;
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