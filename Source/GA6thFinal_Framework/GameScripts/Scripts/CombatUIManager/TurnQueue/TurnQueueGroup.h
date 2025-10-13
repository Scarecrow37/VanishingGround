#pragma once
#include <CombatUIManager/UIGroup.h>
class OverlayPanel;
class ImageElement;

namespace CombatUI
{
    struct TurnQueueGroup : public UIGroup
    {
        struct Slot
        {

        };
        OverlayPanel* GroupPanel = nullptr;
        std::vector<Slot> SlotList;
        size_t            ValidSlotCount = 0;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI