#pragma once
#include "Monster/Action/BossServant/MonsterBossServantAction.h"

namespace Monster
{
    /// <summary>
    /// 검은 성찬
    /// </summary>
    namespace Action
    {
        class BlackSacrament : public BossServantAction
        {
            MONSTER_ACTION_DATA(210270)
        public:
            BlackSacrament();
            ~BlackSacrament() override;

        private:
            void OnActionEnter() override;

        private:
            void Behavior();
        };
    } // namespace Action
} // namespace Monster