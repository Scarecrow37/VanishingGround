#include "pchScripts.h"
#include "EraseRevelationNavi.h"
#include "RevelationSystem/RevelationSystem.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(EraseRevelationNavi)

EraseRevelationNavi::EraseRevelationNavi()
{
    _mySlot = -1;
}
EraseRevelationNavi::~EraseRevelationNavi() = default;

void EraseRevelationNavi::Awake() 
{
    if (ImageElement* focusImage = GetComponent<ImageElement>())
    {
        _focusImage = focusImage->GetWeakPtr();
    }
}

void EraseRevelationNavi::FocusIn(FocusCallType type) 
{
    Base::FocusIn(type);
    SetEnableFocusImage(true);
}

void EraseRevelationNavi::Submit() 
{
    Base::Submit();
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        system->RemovePlayerElement(_mySlot);
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
