#include "pchScripts.h"
#include "RevelationsGroup.h"


#include <UI/Views/RevelationsView/RevelationsView.h>
#include <UI/Animations/FadeUIComponent/FadeUIComponent.h>

namespace CombatUI
{
    bool RevelationsGroup::FindUI()
    {
        Root = GameObject::FindWithTag("Revelations Panel").lock().get();
        if (Root)
        {
            if (View = Root->GetComponent<RevelationsView>())
            {
                const auto& revelationUIs = View->GetRevelationUIs();
                for (size_t i = 0; i < revelationUIs.size(); ++i)
                {
                    Slot slot;
                    slot.IconElement        = revelationUIs[i].IconElement;
                    slot.NameElement        = revelationUIs[i].NameElement;
                    slot.DescriptionElement = revelationUIs[i].DescriptionElement;
                    slot.GradeElements      = revelationUIs[i].GradeElements;
                    SlotList[i] = slot;
                }
            }
            FadeUI = Root->GetComponent<FadeUIComponent>();
        }
        return IsValid();
    }

    bool RevelationsGroup::IsValid() const
    {
        return Root && View;
    }

    void RevelationsGroup::ActiveUI(bool active) 
    {
        if (Root)
        {
            Root->ActiveSelf = active;
        }
    }

    void RevelationsGroup::FadeIn(float duration)
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

    void RevelationsGroup::FadeOut(float duration)
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