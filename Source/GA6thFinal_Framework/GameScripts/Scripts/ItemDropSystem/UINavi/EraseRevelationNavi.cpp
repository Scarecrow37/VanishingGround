#include "pchScripts.h"
#include "EraseRevelationNavi.h"
#include "ItemDropSystem/UI/EraseRevelationUIManager.h"
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
        focusImage->Enable = false;
        _focusImage        = focusImage->GetWeakPtr();
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
    if (EraseRevelationUIManager* system = SingletonComponent<EraseRevelationUIManager>::GetInstance())
    {
        system->EraseRevelation(_mySlot);
        UmLogger.Message(LogLevel::LEVEL_TRACE, u8"계시 지우기 버튼 눌림!");
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
