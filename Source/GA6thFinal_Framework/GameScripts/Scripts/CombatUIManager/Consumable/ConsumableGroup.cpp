#include "pchScripts.h"
#include "ConsumableGroup.h"

#include <UI/Panels/Grid/GridPanel.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>

namespace CombatUI
{
    bool ConsumableGroup::FindUI()
    {
        Root = nullptr;
        
        if (Root = GameObject::FindWithTag("Consumable Panel").lock().get())
        {
            // 그룹 패널 내부를 탐색
            Transform& transform  = Root->transform;
            size_t     childCount = static_cast<size_t>(transform.GetChildCount());
            for (size_t i = 0; i < childCount; ++i)
            {
                if (Transform* child = transform.GetChild((int)i))
                {
                    if (child->gameObject->CompareTag("Focus Grid Panel"))
                    {
                        FocusPanel = child->gameObject->GetComponent<GridPanel>();
                    }
                    else if (child->gameObject->CompareTag("Icon Grid Panel"))
                    {
                        IconPanel = child->gameObject->GetComponent<GridPanel>();
                    }
                }
            }
        }
        return IsValid();
    }

    bool ConsumableGroup::IsValid() const
    {
        return Root && FocusPanel && IconPanel;
    }

    void ConsumableGroup::ActiveUI(bool active) 
    {
        if (Root)
        {
            Root->ActiveSelf = active;
        }
    }
} // namespace CombatUI