#pragma once
#include <CombatUIManager/UIGroup.h>

class OverlayPanel;
class ImageElement;
class RevelationsView;

namespace CombatUI
{
    struct RevelationsGroup : public UIGroup
    {
        struct Slot
        {

        };
        OverlayPanel* GroupPanel = nullptr;
        RevelationsView* RevelationsUI = nullptr;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI