#include "pch.h"
#include "TimelineEventContext.h"

namespace Timeline
{
    void EventContext::SetGameObject(std::weak_ptr<GameObject> weakObj) 
    {
        _gameObject = weakObj;
    }
    void EventContext::SetEvent(std::string_view typeNameID) 
    {
        ReflectFields->EventType = typeNameID;
    }
    void EventContext::SetTime(float time) 
    {
        ReflectFields->Time = time;
    }
    void EventContext::SetLabel(std::string_view label) 
    {
        ReflectFields->Label = label;
    }
    bool EventContext::IsValidID() const
    {
        return ReflectFields->ContextID != UINT_MAX;
    }
    bool EventContext::IsSameEvent(const EventContext* other) const
    {
        if (other)
        {
            return ReflectFields->EventType == other->ReflectFields->EventType;
        }
        return false;
    }
}