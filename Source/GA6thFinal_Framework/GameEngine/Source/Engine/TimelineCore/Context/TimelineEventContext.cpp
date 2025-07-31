#include "pch.h"
#include "TimelineEventContext.h"

namespace Timeline
{
    void EventContext::SetEvent(std::string_view typeNameID) 
    {
        ReflectFields->EventType = typeNameID;
        RequireEvent(typeNameID);
    }
    void EventContext::SetTime(float time) 
    {
        ReflectFields->Time = time;
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