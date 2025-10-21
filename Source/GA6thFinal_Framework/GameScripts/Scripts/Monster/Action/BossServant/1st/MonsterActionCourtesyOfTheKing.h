#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 왕에 대한 예우
    /// </summary>
    namespace Action
    {
        class CourtesyOfTheKing : public Base
        {
            MONSTER_ACTION_DATA(210271)
        public:
            CourtesyOfTheKing();
            ~CourtesyOfTheKing() override;

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