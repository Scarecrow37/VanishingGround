#pragma once
#include "Monster/Action/BossServant/MonsterBossServantAction.h"

namespace Monster
{
    /// <summary>
    /// 왕에 대한 예우
    /// </summary>
    namespace Action
    {
        class CourtesyOfTheKing : public BossServantAction
        {
            MONSTER_ACTION_DATA(210271)
        public:
            CourtesyOfTheKing();
            ~CourtesyOfTheKing() override;

        private:
            void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;

        private:
            void Behavior();
        };
    } // namespace Action
} // namespace Monster