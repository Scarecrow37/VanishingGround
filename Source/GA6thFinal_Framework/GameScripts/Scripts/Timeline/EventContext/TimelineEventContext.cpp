#include "pchScripts.h"
#include "TimelineEventContext.h"
#include <TurnSystem/TurnAction/TurnAction.h>

namespace Timeline
{
    ActionContext::ActionContext(UINT id) 
        : EventContextBase(id)
        , _event(nullptr)
    {
    }
    ActionContext::~ActionContext()
    {
        if (_event)
        {
            delete _event;
            _event = nullptr;
        }
    }
    void ActionContext::SetEvent(std::string_view typeNameID) 
    {
        if (nullptr != _event)
        {
            delete _event;
            _event = nullptr;
        }
        ReflectFields->EventNameData = typeNameID;
        _event = FactoryConstructor<TurnAction>::NewInstanceWithKey(ReflectFields->EventNameData);
    }
    void ActionContext::SetTime(float time)
    {
        ReflectFields->TimeData = time;
    }
    void ActionContext::SerializedReflectEvent() 
    {
        if (nullptr != _event)
        {
            ReflectFields->SerializedData = _event->SerializedReflectFields();
        }
    }
    void ActionContext::DeserializedReflectEvent() 
    {
        _event = FactoryConstructor<TurnAction>::NewInstanceWithKey(ReflectFields->EventNameData);
        if (nullptr != _event)
        {
            _event->DeserializedReflectFields(ReflectFields->SerializedData);
        }
    }
} // namespace Timeline