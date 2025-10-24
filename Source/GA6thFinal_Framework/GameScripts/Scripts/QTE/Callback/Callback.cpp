#include "pchScripts.h"
#include "Callback.h"

namespace QTE
{
    Callback::Handle CallbackHandler::RegisterCallback(const Callback& callback)
    {
        Callback::Handle handle = ++_handleCounter;
        _callbackTable[handle] = callback;
        return handle;
    }

    bool CallbackHandler::UnRegisterCallback(Callback::Handle handle)
    {
        if (_callbackTable.contains(handle))
        {
            _callbackTable.erase(handle);
            return true;
        }
        return false;
    }

    void CallbackHandler::ProcessQTEFadeInStartEvent() 
    {
        for (auto& [_, callback] : _callbackTable)
        {
            if (callback.OnFadeInStart && false == callback.Invalid())
            {
                callback.OnFadeInStart();
            }
        }
    }

    void CallbackHandler::ProcessQTEFadeInFinishEvent() 
    {
        for (auto& [_, callback] : _callbackTable)
        {
            if (callback.OnFadeInFinish && false == callback.Invalid())
            {
                callback.OnFadeInFinish();
            }
        }
    }

    void CallbackHandler::ProcessQTEFadeOutStartEvent(const QTE::OverallResult& result) 
    {
        for (auto& [_, callback] : _callbackTable)
        {
            if (callback.OnFadeOutStart && false == callback.Invalid())
            {
                callback.OnFadeOutStart(result);
            }
        }
    }

    void CallbackHandler::ProcessQTEFadeOutFinishEvent(const QTE::OverallResult& result) 
    {
        for (auto& [_, callback] : _callbackTable)
        {
            if (callback.OnFadeOutFinish && false == callback.Invalid())
            {
                callback.OnFadeOutFinish(result);
            }
        }
    }

    void CallbackHandler::ProcessQTEButtonPressedEvent()
    {
        for (auto& [_, callback] : _callbackTable)
        {
            if (callback.OnButtonPressed && false == callback.Invalid())
            {
                callback.OnButtonPressed();
            }
        }
    }
    void CallbackHandler::ProcessQTENotePressedEvent(UINT noteID, QTE::ResultType result)
    {
        for (auto& [_, callback] : _callbackTable)
        {
            if (callback.OnNotePressed && false == callback.Invalid())
            {
                callback.OnNotePressed(noteID, result);
            }
        }
    }

} // namespace QTE