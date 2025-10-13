#include "pchScripts.h"
#include "ConsumableGroup.h"

#include <UI/Panels/Grid/GridPanel.h>
#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>

namespace CombatUI
{
    bool ConsumableGroup::FindUI()
    {
        SlotList.clear();
        GroupPanel = nullptr;
        auto group = GameObject::FindWithTag("Consumable Panel").lock();
        if (group)
        {
            GroupPanel = group->GetComponent<OverlayPanel>();
            // 그룹 패널 내부를 탐색
            Transform& transform  = GroupPanel->transform;
            size_t     childCount = static_cast<size_t>(transform.GetChildCount());
            SlotList.reserve(childCount);
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

            if (FocusPanel && IconPanel)
            {
                Transform& focusTransform = FocusPanel->transform;
                Transform& iconTransform  = IconPanel->transform;

                for (int i = 0; i < focusTransform.GetChildCount(); ++i)
                {
                    if (Transform* child = focusTransform.GetChild(i))
                    {
                        if (child->gameObject->CompareTag("Focus"))
                        {
                            Slot slot;
                            slot.FocusImage = child->gameObject->GetComponent<ImageElement>();
                            SlotList.push_back(slot);
                        }
                    }
                }
                for (int i = 0; i < iconTransform.GetChildCount(); ++i)
                {
                    if (Transform* child = iconTransform.GetChild(i))
                    {
                        if (child->gameObject->CompareTag("Icon"))
                        {
                            if (i < SlotList.size())
                            {
                                SlotList[i].IconImage = child->gameObject->GetComponent<ImageElement>();
                            }
                        }
                    }
                }

                for (const auto& slot : SlotList)
                {
                    if (slot.IsValid())
                    {
                        ++ValidSlotCount;
                    }
                }
            }
        }
        return IsValid();
    }

    bool ConsumableGroup::IsValid() const
    {
        return GroupPanel && FocusPanel && IconPanel && SlotList.size() == ValidSlotCount;
    }

    void ConsumableGroup::ActiveUI(bool active) 
    {
        if (GroupPanel)
        {
            GroupPanel->gameObject->ActiveSelf = active;
        }
    }
} // namespace CombatUI