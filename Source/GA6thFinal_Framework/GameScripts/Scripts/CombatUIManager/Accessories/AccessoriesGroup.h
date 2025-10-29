#pragma once
#include <CombatUIManager/UIGroup.h>
#include <vector>

class GridPanel;
class OverlayPanel;
class ImageElement;
class AccessoriesView;
class FadeUIComponent;

namespace CombatUI
{
    struct AccessoriesGroup : public UIGroup
    {
        struct Slot
        {
            ImageElement*   FrameImage  = nullptr;
            ImageElement*   IconImage   = nullptr;
            inline bool     IsValid() const { return FrameImage && IconImage; }
        };

        GameObject*         Root = nullptr;
        AccessoriesView*    View        = nullptr;
        std::vector<Slot>   SlotList;
        size_t              ValidSlotCount = 0;
        FadeUIComponent*    FadeUI         = nullptr;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
        void FadeIn(float duration) override;
        void FadeOut(float duration) override;
    };
} // namespace CombatUI