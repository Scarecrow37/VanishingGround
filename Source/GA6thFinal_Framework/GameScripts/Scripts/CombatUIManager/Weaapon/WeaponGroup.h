#pragma once
#include <CombatUIManager/UIGroup.h>
class OverlayPanel;
class ImageElement;

namespace CombatUI
{
    struct WeaponGroup : public UIGroup
    {
        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI