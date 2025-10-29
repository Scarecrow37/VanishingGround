#pragma once
#include <CombatUIManager/UIGroup.h>

class GridPanel;
class OverlayPanel;
class ImageElement;
class FadeUIComponent;

namespace CombatUI
{
    struct ConsumableGroup : public UIGroup
    {
        GameObject*     Root        = nullptr;
        GridPanel*      FocusPanel  = nullptr;
        GridPanel*      IconPanel   = nullptr;

        FadeUIComponent* FadeUI     = nullptr;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;

        // UIGroup을(를) 통해 상속됨
        void FadeIn(float duration) override;
        void FadeOut(float duration) override;
    };
} // namespace CombatUI