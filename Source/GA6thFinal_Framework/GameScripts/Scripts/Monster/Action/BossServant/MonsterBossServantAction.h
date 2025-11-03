#pragma once
#include "Monster/Action/MonsterActionBase.h"

namespace Monster
{
    namespace Action
    {
        class BossServantAction : public Base
        {
        public:
            using Base::Base;

        private:
            virtual void OnActionExit() override;
        };
    }
} // namespace Monster