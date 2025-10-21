#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 기록 말살
    /// </summary>
    namespace Action
    {
        class RecordObliteration : public Base
        {
            MONSTER_ACTION_DATA(210261)
        public:
            RecordObliteration();
            ~RecordObliteration() override;
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