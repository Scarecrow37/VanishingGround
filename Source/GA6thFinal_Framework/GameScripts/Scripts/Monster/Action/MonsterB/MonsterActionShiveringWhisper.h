#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 떨리는 중얼거림
    /// </summary>
    namespace Action
    {
        class ShiveringWhisper : public Base
        {
            MONSTER_ACTION_DATA(210211)
        public:
            ShiveringWhisper();
            ~ShiveringWhisper() override;

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