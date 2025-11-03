#pragma once
#include "Monster/Action/BossServant/MonsterBossServantAction.h"

namespace Monster
{
    namespace Action
    {
        /// <summary>
        /// 전가
        /// </summary>
        class Impute : public BossServantAction
        {
            MONSTER_ACTION_DATA(210281)
        public:
            Impute();
            ~Impute() override;

        private:
            void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;

        private:
            void Attack();
        };
    } // namespace Action
} // namespace Monster
