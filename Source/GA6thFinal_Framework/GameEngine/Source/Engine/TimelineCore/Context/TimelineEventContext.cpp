#include "pch.h"
#include "TimelineEventContext.h"

namespace Timeline
{
    void EventContext::SetEvent(std::string_view typeNameID) 
    {
        ReflectFields->EventNameData = typeNameID;
        RequireEvent(typeNameID);
    }
    void EventContext::SetTime(float time) 
    {
        ReflectFields->TimeData = time;
    }
    bool EventContext::IsValidID() const
    {
        return ReflectFields->ContextID != UINT_MAX;
    }
    bool EventContext::IsSameEvent(const EventContext* other) const
    {
        if (other)
        {
            return ReflectFields->EventNameData == other->ReflectFields->EventNameData;
        }
        return false;
    }
}