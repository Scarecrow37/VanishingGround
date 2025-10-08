#pragma once
#include <CombatUIManager/UIGroup.h>
class OverlayPanel;
class ImageElement;

namespace CombatUI
{
    struct ConsumableGroup : public UIGroup
    {
        OverlayPanel* GroupPanel = nullptr;
    
        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI