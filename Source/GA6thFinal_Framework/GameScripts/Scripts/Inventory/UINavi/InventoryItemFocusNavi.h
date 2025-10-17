#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"

class ImageElement; 
class InventoryItemFocusNavi : public UISFXNavigationComponent
{
    USING_PROPERTY(InventoryItemFocusNavi)

public:
    inline static constexpr const char* TAG = "Focus";

    InventoryItemFocusNavi();
    ~InventoryItemFocusNavi() override;

    void SetItemInfo(const DropItemInfo& item);

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(InventoryItemFocusNavi)

    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;

    void Awake() override;

    ImageElement* _focusImage;
    DropItemInfo  _itemInfo;
};

