#pragma once
#include <CombatUIManager/UIGroup.h>

class GridPanel;
class OverlayPanel;
class ImageElement;

namespace CombatUI
{
    struct ConsumableGroup : public UIGroup
    {
        GameObject*     Root        = nullptr;
        GridPanel*      FocusPanel  = nullptr;
        GridPanel*      IconPanel   = nullptr;
        
        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI