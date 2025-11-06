#pragma once
#include <CombatUIManager/UIGroup.h>

class OverlayPanel;
class ImageElement;
class TurnQueueView;
class FadeUIComponent;

namespace CombatUI
{
    struct TurnQueueGroup : public UIGroup
    {
        struct Slot
        {
            ImageElement* Frame         = nullptr;
            ImageElement* Portraits     = nullptr;
            ImageElement* ButtonXIcon   = nullptr;
            ImageElement* ButtonYIcon   = nullptr;
            ImageElement* ButtonBIcon   = nullptr;
        };
        GameObject*       Root = nullptr;
        TurnQueueView*    View = nullptr;
        std::vector<Slot> SlotList;
        FadeUIComponent*  FadeUI = nullptr;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
        void FadeIn(float duration) override;
        void FadeOut(float duration) override;
    };
} // namespace CombatUI