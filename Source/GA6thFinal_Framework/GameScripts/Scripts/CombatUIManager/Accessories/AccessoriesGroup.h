#pragma once
#include <CombatUIManager/UIGroup.h>
#include <vector>

class GridPanel;
class OverlayPanel;
class ImageElement;

namespace CombatUI
{
    struct AccessoriesGroup : public UIGroup
    {
        struct Slot
        {
            OverlayPanel*   MainPanel   = nullptr;
            ImageElement*   FrameImage  = nullptr;
            ImageElement*   IconImage   = nullptr;
            inline bool     IsValid() const { return MainPanel && FrameImage && IconImage; }
        };

        OverlayPanel*       GroupPanel = nullptr;
        std::vector<Slot>   SlotList;
        size_t              ValidSlotCount = 0;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI