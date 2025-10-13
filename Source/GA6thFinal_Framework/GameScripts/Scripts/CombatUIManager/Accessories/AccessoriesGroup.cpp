#include "pchScripts.h"
#include "AccessoriesGroup.h"

#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Elements/Image/ImageElement.h>

namespace CombatUI
{
    bool AccessoriesGroup::FindUI()
    {
        SlotList.clear();
        ValidSlotCount = 0;

        auto hudGroup = GameObject::FindWithTag("Accessories Panel").lock();
        if (hudGroup)
        {
            GroupPanel = hudGroup->GetComponent<OverlayPanel>();

            // 그룹 패널 내부를 탐색
            Transform& transform  = hudGroup->transform;
            size_t     childCount = static_cast<size_t>(transform.GetChildCount());
            SlotList.reserve(childCount);
            for (size_t i = 0; i < childCount; ++i)
            {
                if (Transform* child = transform.GetChild((int)i))
                {
                    if (OverlayPanel* panel = child->gameObject->GetComponent<OverlayPanel>())
                    {
                        Slot slot;
                        slot.MainPanel = panel;
                        Transform::ForeachBFS(*child, [&slot](Transform* curr) {
                            if (curr->gameObject->CompareTag("Frame"))
                            {
                                slot.FrameImage = curr->gameObject->GetComponent<ImageElement>();
                            }
                            else if (curr->gameObject->CompareTag("Icon"))
                            {
                                slot.IconImage = curr->gameObject->GetComponent<ImageElement>();
                            }
                        });
                        // 유효한 슬롯일 경우 카운트 증가
                        if (slot.IsValid())
                        {
                            ++ValidSlotCount;
                        }
                        SlotList.push_back(slot);
                    }
                }
            }
        }

        return IsValid();
    }

    bool AccessoriesGroup::IsValid() const
    {
        return GroupPanel && SlotList.size() == ValidSlotCount;
    }

    void AccessoriesGroup::ActiveUI(bool active) 
    {
        if (IsValid())
        {
            GroupPanel->gameObject->ActiveSelf = active;
        }
    }
}
