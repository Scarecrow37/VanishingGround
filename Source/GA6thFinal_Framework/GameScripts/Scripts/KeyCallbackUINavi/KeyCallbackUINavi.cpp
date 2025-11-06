#include "pchScripts.h"
#include "KeyCallbackUINavi.h"

UMREAL_COMPONENT(KeyCallbackUINavi)

KeyCallbackUINavi::KeyCallbackUINavi() = default;
KeyCallbackUINavi::~KeyCallbackUINavi() = default;

void KeyCallbackUINavi::Added() 
{
    SetFocusInAudioID("-901001");
}

void KeyCallbackUINavi::FocusIn(FocusCallType callType) 
{
    Base::FocusIn(callType);
    const std::string& key = CallbackKey;
    if (auto find = EventListenerMapFocusIn.find(key); find != EventListenerMapFocusIn.end())
    {
        find->second.Invoke();
    }
}

void KeyCallbackUINavi::FocusOut(FocusCallType callType) 
{
    Base::FocusOut(callType);
    const std::string& key = CallbackKey;
    if (auto find = EventListenerMapFocusOut.find(key); find != EventListenerMapFocusOut.end())
    {
        find->second.Invoke();
    }
}

void KeyCallbackUINavi::Submit()
{
    Base::Submit();
    const std::string& key = CallbackKey;
    if (auto find = EventListenerMapSubmit.find(key); find != EventListenerMapSubmit.end())
    {
        find->second.Invoke();
    }
}

void KeyCallbackUINavi::ShowTooltips() 
{
    const std::string& key = CallbackKey;
    if (auto find = EventListenerMapShowTooltips.find(key); find != EventListenerMapShowTooltips.end())
    {
        find->second.Invoke();
    }
}

void KeyCallbackUINavi::HideTooltips() 
{
    const std::string& key = CallbackKey;
    if (auto find = EventListenerMapHideTooltips.find(key); find != EventListenerMapHideTooltips.end())
    {
        find->second.Invoke();
    }
}

