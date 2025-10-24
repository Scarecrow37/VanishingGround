#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 두려움의 중얼거림
    /// </summary>
    namespace Action
    {
        class WhisperOfFear : public Base
        {
            MONSTER_ACTION_DATA(210210)
        public:
            WhisperOfFear();
            ~WhisperOfFear() override;

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