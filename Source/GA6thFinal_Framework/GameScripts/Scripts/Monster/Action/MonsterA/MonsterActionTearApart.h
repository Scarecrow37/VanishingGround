#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    /// <summary>
    /// 찢어 발기기
    /// </summary>
    namespace Action
    {
        class TearApart : public Base
        {
            MONSTER_ACTION_DATA(210202)
        public:
            using Base::Base;

        private:
            void OnActionEnter() override;
            void OnActionUpdate() override;
            void OnActionExit() override;
            void OnActionReset() override;

        private:
        };
    }
} // namespace Monster