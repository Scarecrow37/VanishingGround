#include "pchScripts.h"
#include "AccessoriesGroup.h"

#include <UI/Panels/Grid/GridPanel.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>

#include <UI/Views/Accessories/AccessoriesView.h>

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
}
