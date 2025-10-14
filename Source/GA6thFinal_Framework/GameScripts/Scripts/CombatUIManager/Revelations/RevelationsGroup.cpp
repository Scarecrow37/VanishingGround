#include "pchScripts.h"
#include "RevelationsGroup.h"


#include <UI/Views/RevelationsView/RevelationsView.h>

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
} // namespace CombatUI