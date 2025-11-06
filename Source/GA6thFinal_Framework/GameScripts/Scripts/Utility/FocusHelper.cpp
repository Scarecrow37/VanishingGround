#include "pchScripts.h"
#include "FocusHelper.h"
#include "SingletonHelper.h"
#include "Inventory/UI/InventoryUIManager.h"
#include "Preferences/PreferencesManager.h"

namespace FocusHelper
{
    bool CheckFocus(FocusHelper::Flags flags)
    {
        if ((flags & FocusHelper::IS_FOCUSED_INVENTORY) != 0)
        {
            if (auto* manager = SingletonComponent<InventoryUIManager>::GetInstance())
            {
                if (manager->gameObject->ActiveInHierarchy)
                    return true;
            }
        }

        if ((flags & FocusHelper::IS_FOCUSED_PREFERENCES) != 0)
        {
            if (auto* manager = SingletonComponent<PreferencesManager>::GetInstance())
            {
                if (manager->IsOpen())
                    return true;
            }
        }

        return false;
    }
}