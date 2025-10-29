#include "pchScripts.h"
#include "WeaponGroup.h"

#include <UI/Views/Weapon/WeaponView.h>
#include <UI/Animations/FadeUIComponent/FadeUIComponent.h>

namespace CombatUI
{
    bool WeaponGroup::FindUI()
    {
        if (Root = GameObject::FindWithTag("Weapon Panel").lock().get())
        {
            if (View = Root->GetComponent<WeaponView>())
            {
                Icon = View->IconUI;
                Name = View->NameUI;
                Description = View->DescriptionUI;
                const WeaponView::BackgroundUI& background = View->BackgroundUIInfo;
                Background.FocusOn      = background.FocusOn;
                Background.FocusOff     = background.FocusOff;

                const WeaponView::TextInfoUI& textInfo = View->TextInfo;
                TextInfo.Damage         = textInfo.Damage;
                TextInfo.Critical       = textInfo.Critical;
                TextInfo.AttackCount    = textInfo.AttackCount;
                TextInfo.Speed          = textInfo.Speed;
            }
            FadeUI = Root->GetComponent<FadeUIComponent>();
        }

        return IsValid();
    }

    bool WeaponGroup::IsValid() const
    {
        return Root && View && Icon && Name && Description && Background.IsValid() && TextInfo.IsValid();
    }

    void WeaponGroup::ActiveUI(bool active) 
    {
        if (Root)
        {
            Root->ActiveSelf = active;
        }
    }

    void WeaponGroup::FadeIn(float duration)
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

    void WeaponGroup::FadeOut(float duration)
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