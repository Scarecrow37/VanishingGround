#include "pchScripts.h"
#include "InventoryItemFocusNavi.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(InventoryItemFocusNavi)

InventoryItemFocusNavi::InventoryItemFocusNavi() = default;
InventoryItemFocusNavi::~InventoryItemFocusNavi() = default;

void InventoryItemFocusNavi::FocusIn(FocusCallType callType) 
{
    Base::FocusIn(callType);
    if (_focusImage)
    {
        _focusImage->Enable = true;
    }
}

void InventoryItemFocusNavi::Submit() 
{
    Base::Submit();

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
    if (_focusImage)
    {
        _focusImage->Enable = false;
    }
    gameObject->AddTag(TAG);
}

