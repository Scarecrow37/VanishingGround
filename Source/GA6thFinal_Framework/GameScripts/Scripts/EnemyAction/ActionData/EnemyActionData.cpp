#include "pchScripts.h"
#include "EnemyActionData.h"

namespace EnemyAction
{
    void ActionData::SerializedReflectEvent()
    {
        if (EventTrack)
        {
            ReflectFields->EventTrackSerializedData = EventTrack->SerializedReflectFields();
        }
    }

    void ActionData::DeserializedReflectEvent()
    {
        if (EventTrack)
        {
            EventTrack->DeserializedReflectFields(ReflectFields->EventTrackSerializedData);
        }
    }

    void ActionData::ImGuiDrawPropertysEvent() 
    {
    }
   
} // namespace EnemyAction

REGISTER_CLASS(Timeline::EventTrack, TimelineEvent);
TimelineEvent::TimelineEvent() {}