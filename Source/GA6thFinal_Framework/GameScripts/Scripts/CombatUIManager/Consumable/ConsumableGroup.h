#pragma once
#include <CombatUIManager/UIGroup.h>

class GridPanel;
class OverlayPanel;
class ImageElement;

namespace CombatUI
{
    struct ConsumableGroup : public UIGroup
    {
        struct Slot
        {
            ImageElement* FocusImage    = nullptr;
            ImageElement* IconImage     = nullptr;
            inline bool   IsValid() const { return FocusImage && IconImage; }
        };
        OverlayPanel* GroupPanel = nullptr;
        GridPanel*    FocusPanel = nullptr;
        GridPanel*    IconPanel  = nullptr;
        std::vector<Slot> SlotList;
        size_t            ValidSlotCount = 0;
        
        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI