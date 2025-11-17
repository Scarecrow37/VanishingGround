#include "pchScripts.h"
#include "InventoryItemFocusNavi.h"
#include "UI/Elements/Image/ImageElement.h"
#include "Inventory/UI/InventoryUIManager.h"
#include "ItemDropSystem/UI/ItemInfoUIManager.h"

UMREAL_COMPONENT(InventoryItemFocusNavi)

InventoryItemFocusNavi::InventoryItemFocusNavi()
{
    _itemInfo.Category = ArtifactDropType::SWORD;
    _itemInfo.ID       = 0;
    _itemInfo.Name     = STR_NULL;
    _weaponStats       = {};

    SetFocusInAudioID("-901001");
}
InventoryItemFocusNavi::~InventoryItemFocusNavi() = default;

void InventoryItemFocusNavi::FocusIn(FocusCallType callType) 
{
    Base::FocusIn(callType);
    if (_focusImage)
    {
        _focusImage->Enable = true;
    }
    if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
    {
        if (ItemInfoUIManager* infoManager = manager->GetItemInfoManager())
        {
            infoManager->SetItemInfoUI(_itemInfo);
            ArtifactDropType category = _itemInfo.Category;
            switch (category)
            {
            case ArtifactDropType::SWORD:
            case ArtifactDropType::DAGGER:
            case ArtifactDropType::WARHAMMER:
                infoManager->SetWeaponStats(_weaponStats);
                break;
            default:
                break;
            }
        }
    }
}

void InventoryItemFocusNavi::FocusOut(FocusCallType callType) 
{
    Base::FocusOut(callType);
    if (_focusImage)
    {
        _focusImage->Enable = false;
    }
}

void InventoryItemFocusNavi::Awake() 
{
    Base::Awake();
    _focusImage = GetComponent<ImageElement>();
    gameObject->AddTag(TAG);
}

void InventoryItemFocusNavi::SetItemInfo(const DropItemInfo& item)
{
    _itemInfo = item;
}

void InventoryItemFocusNavi::SetWeaponStats(const WeaponStats& stats)
{
    _weaponStats = stats;
}
