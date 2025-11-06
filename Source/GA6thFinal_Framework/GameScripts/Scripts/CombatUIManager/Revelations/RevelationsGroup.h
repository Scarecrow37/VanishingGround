#pragma once
#include <CombatUIManager/UIGroup.h>

class DescriptionPanel;
class ImageElement;
class TextElement;
class RevelationsView;
class FadeUIComponent;

namespace CombatUI
{
    struct RevelationsGroup : public UIGroup
    {
        struct Slot
        {
            ImageElement*              IconElement          = nullptr;
            std::vector<ImageElement*> GradeElements;
            TextElement*               NameElement          = nullptr;
            DescriptionPanel*          DescriptionElement   = nullptr;

            inline bool IsValid() const { return IconElement && NameElement && DescriptionElement && false == GradeElements.empty(); }
        };
        GameObject*         Root = nullptr;
        RevelationsView*    View = nullptr;

        std::array<Slot, 3> SlotList;
        FadeUIComponent*    FadeUI = nullptr;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
        void FadeIn(float duration) override;
        void FadeOut(float duration) override;
    };
} // namespace CombatUI