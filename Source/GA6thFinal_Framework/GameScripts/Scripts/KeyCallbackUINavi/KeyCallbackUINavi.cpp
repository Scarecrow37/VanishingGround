#include "pchScripts.h"
#include "KeyCallbackUINavi.h"

UMREAL_COMPONENT(KeyCallbackUINavi)

KeyCallbackUINavi::KeyCallbackUINavi() = default;
KeyCallbackUINavi::~KeyCallbackUINavi() = default;

std::pair<UmDelegate<>*, UmDelegate<>::Handle> KeyCallbackUINavi::AddCallback(UmDelegate<>& listener, const std::function<void()>& callback)
{
    UmDelegate<>::Handle handel = listener.AddListener(callback);
    return std::make_pair(&listener, handel);
}

std::pair<UmDelegate<>*, UmDelegate<>::Handle> KeyCallbackUINavi::AddCallbackFocusIn(
    const std::string& key, const std::function<void()>& callback)
{
    return AddCallback(EventListenerMapFocusIn[key], callback);
}

std::pair<UmDelegate<>*, UmDelegate<>::Handle> KeyCallbackUINavi::AddCallbackFocusOut(
    const std::string& key, const std::function<void()>& callback)
{
    return AddCallback(EventListenerMapFocusOut[key], callback);
}

std::pair<UmDelegate<>*, UmDelegate<>::Handle> KeyCallbackUINavi::AddCallbackSubmit(
    const std::string& key, const std::function<void()>& callback)
{
    return AddCallback(EventListenerMapSubmit[key], callback);
}

std::pair<UmDelegate<>*, UmDelegate<>::Handle> KeyCallbackUINavi::AddCallbackShowTooltips(
    const std::string& key, const std::function<void()>& callback)
{
    return AddCallback(EventListenerMapShowTooltips[key], callback);
}

std::pair<UmDelegate<>*, UmDelegate<>::Handle> KeyCallbackUINavi::AddCallbackHideTooltips(
    const std::string& key, const std::function<void()>& callback)
{
    return AddCallback(EventListenerMapHideTooltips[key], callback);
}

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

