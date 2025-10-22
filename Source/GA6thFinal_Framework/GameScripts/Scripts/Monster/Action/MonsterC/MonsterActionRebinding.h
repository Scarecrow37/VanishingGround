#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 재결속
    /// </summary>
    namespace Action
    {
        class Rebinding : public Base
        {
            MONSTER_ACTION_DATA(210223)
        public:
            Rebinding();
            ~Rebinding() override;

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