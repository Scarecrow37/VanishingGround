#include "pch.h"
#include "TimelineEventContext.h"

namespace Timeline
{
    EventContextBase::EventContextBase(UINT id)
        : ReflectSerializer()
    {
        ReflectFields->TimeData       = 0.0f;
        ReflectFields->NotifyID       = id;
        ReflectFields->EventNameData  = "";
        ReflectFields->SerializedData = "";
    }
    EventContextBase::~EventContextBase()
    {
        // Do nothing
    }
    bool EventContextBase::IsValidID() const
    {
        return ReflectFields->NotifyID != UINT_MAX;
    }
}