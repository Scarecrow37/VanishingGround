#include "pchScripts.h"
#include "WeaponGroup.h"

#include <UI/Views/Weapon/WeaponView.h>

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
                Background.OnImage      = background.ImageOn;
                Background.OffImage     = background.ImageOff;

                const WeaponView::TextInfoUI& textInfo = View->TextInfo;
                TextInfo.Damage         = textInfo.Damage;
                TextInfo.Critical       = textInfo.Critical;
                TextInfo.AttackCount    = textInfo.AttackCount;
                TextInfo.Speed          = textInfo.Speed;
            }
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
} // namespace CombatUI