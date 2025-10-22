#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 찢어 발기기
    /// </summary>
    namespace Action
    {
        class RendApart : public Base
        {
            MONSTER_ACTION_DATA(210200)
        public:
            RendApart();
            ~RendApart() override;

        private:
            void OnActionEnter() override;
            void OnActionUpdate() override;
            void OnActionExit() override;
            void OnActionReset() override;
            void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;

        private:
            void Attack();
        };
    }
} // namespace Monster