#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 검은 성찬
    /// </summary>
    namespace Action
    {
        class BlackSacrament : public Base
        {
            MONSTER_ACTION_DATA(210270)
        public:
            BlackSacrament();
            ~BlackSacrament() override;

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