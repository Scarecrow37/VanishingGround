#pragma once
#include <CombatUIManager/UIGroup.h>

namespace CombatUI
{
    struct RevelationsGroup : public UIGroup
    {
        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI