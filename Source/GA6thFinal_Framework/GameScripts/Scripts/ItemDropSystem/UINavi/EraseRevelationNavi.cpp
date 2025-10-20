#include "pchScripts.h"
#include "EraseRevelationNavi.h"
#include "ItemDropSystem/UI/EraseRevelationUIManager.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(EraseRevelationNavi)

EraseRevelationNavi::EraseRevelationNavi()
{
    _mySlot            = -1;
    _itemInfo.Category = ArtifactDropType::ACCESSORY;
    _itemInfo.ID       = -1;
    _itemInfo.Name     = STR_NULL;
}
EraseRevelationNavi::~EraseRevelationNavi() = default;

void EraseRevelationNavi::Awake() 
{
    if (ImageElement* focusImage = GetComponent<ImageElement>())
    {
        focusImage->Enable = false;
        _focusImage        = focusImage->GetWeakPtr();
    }
}

void EraseRevelationNavi::FocusIn(FocusCallType type) 
{
    Base::FocusIn(type);
    SetEnableFocusImage(true);
    if (EraseRevelationUIManager* system = SingletonComponent<EraseRevelationUIManager>::GetInstance())
    {
        system->SetRevelationInfoUI(_itemInfo);
    }
}

void EraseRevelationNavi::Submit() 
{
    Base::Submit();
    if (EraseRevelationUIManager* system = SingletonComponent<EraseRevelationUIManager>::GetInstance())
    {
        system->EraseRevelation(_mySlot);
    }
}

void EraseRevelationNavi::FocusOut(FocusCallType type) 
{
    Base::FocusOut(type);
    SetEnableFocusImage(false);
}

void EraseRevelationNavi::SetEnableFocusImage(bool enable) 
{
    if (auto focusComponent = _focusImage.lock())
    {
        ImageElement* focusImage = static_cast<ImageElement*>(focusComponent.get());
        if (focusImage)
        {
            focusImage->Enable = enable;
        }
    }
}
