#pragma once
#include "Monster/Action/BossServant/MonsterBossServantAction.h"

namespace Monster
{
    namespace Action
    {
        /// <summary>
        /// 충성의 향유
        /// </summary>
        class EnjoymentOfLoyalty : public BossServantAction
        {
            MONSTER_ACTION_DATA(210280)
        public:
            EnjoymentOfLoyalty();
            ~EnjoymentOfLoyalty() override;

        private:
            void OnActionEnter() override;

        private:
            void Behavior();
        };
    } // namespace Action
} // namespace Monster