#include "pchScripts.h"
#include "AccessoriesGroup.h"

#include <UI/Panels/Grid/GridPanel.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>

#include <UI/Views/Accessories/AccessoriesView.h>
#include <UI/Animations/FadeUIComponent/FadeUIComponent.h>

namespace CombatUI
{
    bool AccessoriesGroup::FindUI()
    {
        SlotList.clear();
        ValidSlotCount = 0;
        Root           = nullptr;

        Root = GameObject::FindWithTag("Accessories Panel").lock().get();
        if (Root)
        {
            if (View = Root->GetComponent<AccessoriesView>())
            {
                const std::vector<AccessoriesView::UI>& uiElements = View->UIElements;
                for (const auto& ui : uiElements)
                {
                    Slot slot = {ui.Frame, ui.Icon};
                    SlotList.push_back(slot);
                    if (slot.IsValid())
                    {
                        ++ValidSlotCount;
                    }
                }
            }
            FadeUI = Root->GetComponent<FadeUIComponent>();
        }

        return IsValid();
    }

    bool AccessoriesGroup::IsValid() const
    {
        return Root && View && SlotList.size() == ValidSlotCount;
    }

    void AccessoriesGroup::ActiveUI(bool active) 
    {
        if (Root)
        {
            Root->ActiveSelf = active;
        }
    }

    void AccessoriesGroup::FadeIn(float duration)
    {
        if (FadeUI)
        {
            FadeUI->FadeDuration = duration;
            FadeUI->FadeIn();
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Fade UI Component가 존재하지 않습니다.");
        }
    }

    void AccessoriesGroup::FadeOut(float duration)
    {
        if (FadeUI)
        {
            FadeUI->FadeDuration = duration;
            FadeUI->FadeOut();
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Fade UI Component가 존재하지 않습니다.");
        }
    }
}
