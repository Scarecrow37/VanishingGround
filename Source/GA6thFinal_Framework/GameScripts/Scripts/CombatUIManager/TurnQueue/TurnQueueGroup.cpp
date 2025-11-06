#include "pchScripts.h"
#include "TurnQueueGroup.h"

#include <UI/Views/TurnQueue/TurnQueueView.h>
#include <UI/Animations/FadeUIComponent/FadeUIComponent.h>

namespace CombatUI
{
    bool TurnQueueGroup::FindUI()
    {
        if (Root = GameObject::FindWithTag("Turn Queue Panel").lock().get())
        {
            if (View = Root->GetComponent<TurnQueueView>())
            {
                const TurnQueueView::ImageElementList& frames       = View->TurnQueueFrames;
                const TurnQueueView::ImageElementList& portraits    = View->TurnQueuePortraits;
                const TurnQueueView::ButtonImageList&  buttonIcons  = View->TurnQueueButtonIcons;
                for (size_t i = 0; i < 6; ++i)
                {
                    Slot slot;
                    slot.Frame       = frames[i];
                    slot.Portraits   = portraits[i];
                    slot.ButtonXIcon = buttonIcons[i].X;
                    slot.ButtonYIcon = buttonIcons[i].Y;
                    slot.ButtonBIcon = buttonIcons[i].B;
                    SlotList.push_back(slot);
                }
            }

            FadeUI = Root->GetComponent<FadeUIComponent>();
        }
        
        return IsValid();
    }

    bool TurnQueueGroup::IsValid() const
    {
        return Root && View;
    }

    void TurnQueueGroup::ActiveUI(bool active) 
    {
        if (Root)
        {
            Root->ActiveSelf = active;
        }
    }

    void TurnQueueGroup::FadeIn(float duration)
    {
        if (false == Root->ActiveSelf)
        {
            ActiveUI(true);
        }

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

    void TurnQueueGroup::FadeOut(float duration) 
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
} // namespace CombatUI