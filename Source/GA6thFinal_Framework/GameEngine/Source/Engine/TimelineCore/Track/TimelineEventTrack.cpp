#include "pch.h"
#include "TimelineEventTrack.h"

namespace Timeline
{
    EventTrack::EventTrack()
    {
        _currFrame = 0.0f;
        _prevFrame = 0.0f;
        _isActive  = true;
        _isPlaying = false;
        ClearContext();
    }
    EventTrack::~EventTrack()
    {
        ClearContext();
    }
    void EventTrack::Update()
    {
        if (GetMaxFrame() < GetMinFrame())
        {
            return;
        }
        if (true == IsActive())
        {
            if (true == IsPlaying() && true == HasFlags(EVENT_TRCK_FLAGS_USE_COUNTER))
            {
                SetCurrentFrame(_currFrame + UmTime.DeltaTime());
            }
            if (true == IsDirty())
            {
                // 이전 프레임이 현재 프레임보다 높다면 최대 값을 넘어가서 돌아왔다고 판단.
                if (_prevFrame > _currFrame)
                {
                    RequestNotify(_prevFrame, GetMaxFrame());
                    RequestNotify(GetMinFrame(), _currFrame);
                }
                else
                {
                    RequestNotify(_prevFrame, _currFrame);
                }
            }
            _prevFrame = _currFrame;
            if (true == IsPlaying() && true == HasFlags(EVENT_TRCK_FLAGS_USE_COUNTER))
            {
                if (_currFrame >= GetMaxFrame())
                {
                    if (true == HasFlags(EVENT_TRCK_FLAGS_LOOP))
                    {
                        _currFrame += GetMinFrame() - GetMaxFrame();
                    }
                    else
                    {
                        SetCurrentFrame(GetMaxFrame(), true);
                        Pause();
                    }
                }
            }
        }
    }
    void EventTrack::Clear()
    {
        SetDefault();
        ClearContext();
    }
    void EventTrack::ClearContext()
    {
        for (auto& context : _contextQueue)
        {
            if (context)
            {
                delete context;
            }
        }
        _contextQueue.clear();
        _contextTable.clear();
    }
    void EventTrack::SetDefault()
    {
        _currFrame = 0.0f;
        _prevFrame = 0.0f;
        _isActive  = true;
        _isPlaying = false;
    }
    void EventTrack::SetActive(bool active)
    {
        _isActive = active;
    }
    void EventTrack::Play()
    {
        _isPlaying = true;
        SetCurrentFrame(GetMinFrame(), true);
    }
    void EventTrack::Stop()
    {
        _isPlaying = false;
        SetCurrentFrame(GetMinFrame(), true);
    }
    void EventTrack::Resume()
    {
        _isPlaying = true;
    }
    void EventTrack::Pause()
    {
        _isPlaying = false;
    }
    EventContext* EventTrack::AddEventEx(std::string_view label, std::string_view typenameID, float time, UINT id)
    {
        UINT uniqueID = (id == UINT_MAX) ? GetUniqueID() : id;
        auto it       = _contextTable.find(uniqueID);
        if (it != _contextTable.end())
        {
            return it->second;
        }
        EventContext* context = NewInstanceWithKey(typenameID);
        if (nullptr == context)
        {
            context = new EventContext();
        }

        if (FLT_MIN == time)
        {
            time = GetCurrentFrame();
        }

        context->SetEvent(typenameID);
        context->SetTime(time);
        context->ReflectFields->Label = label;
        context->ReflectFields->ContextID = uniqueID;
        _contextQueue.push_back(context);
        _contextTable[uniqueID] = context;
        Sort();

        return context;
    }
    EventContext* EventTrack::AddEventExFromCopyBuffer(std::string_view serialData, std::string_view typenameID, float time, UINT id)
    {
        UINT uniqueID = (id == UINT_MAX) ? GetUniqueID() : id;
        auto it       = _contextTable.find(uniqueID);
        if (it != _contextTable.end())
        {
            return it->second;
        }
        EventContext* context = NewInstanceWithKey(typenameID);
        if (nullptr == context)
        {
            context = new EventContext();
        }
        context->DeserializedReflectFields(serialData);
        context->ReflectFields->ContextID = uniqueID;
        context->ReflectFields->Time      = time;
        _contextQueue.push_back(context);
        _contextTable[uniqueID] = context;
        Sort();
        return context;
    }
    bool EventTrack::RemoveContextFromID(UINT id)
    {
        for (auto it = _contextQueue.begin(); it != _contextQueue.end(); ++it)
        {
            auto* context = *it;
            if (context && context->ID == id)
            {
                delete context;
                _contextQueue.erase(it);
                _contextTable.erase(id);
                return true;
            }
        }
        return false;
    }
    bool EventTrack::ChangeContextTime(UINT id, float time)
    {
        EventContext* context = GetContextFromID(id);
        if (nullptr != context)
        {
            context->SetTime(time);
            Sort();
            return true;
        }
        return false;
    }
    bool EventTrack::ChangeContextEvent(UINT id, std::string_view typeNameID)
    {
        EventContext* context = GetContextFromID(id);
        if (nullptr != context)
        {
            context->SetEvent(typeNameID);
            return true;
        }
        return false;
    }
    EventContext* EventTrack::GetContextFromID(UINT id) const
    {
        auto it = _contextTable.find(id);
        if (it != _contextTable.end())
        {
            return it->second;
        }
        return nullptr;
    }
    EventContext* EventTrack::GetContextFromLabel(std::string_view label) const
    {
        for (const auto& context : _contextQueue)
        {
            if (context && context->GetLabel() == label)
            {
                return context;
            }
        }
        return nullptr;
    }
    EventContext* EventTrack::GetNextContextFromID(UINT id) const
    {
        for (size_t i = 0; i < _contextQueue.size(); ++i)
        {
            if (_contextQueue[i] && _contextQueue[i]->ID == id)
            {
                if (i + 1 < _contextQueue.size())
                {
                    return _contextQueue[i + 1];
                }
            }
        }
        return nullptr;
    }
    EventContext* EventTrack::GetPrevContextFromID(UINT id) const
    {
        for (size_t i = 0; i < _contextQueue.size(); ++i)
        {
            if (_contextQueue[i] && _contextQueue[i]->ID == id)
            {
                if (i > 0)
                {
                    return _contextQueue[i - 1];
                }
            }
        }
        return nullptr;
    }
    EventContext* EventTrack::GetBeginContext() const
    {
        if (false == _contextQueue.empty())
        {
            return _contextQueue.front();
        }
        return nullptr;
    }
    EventContext* EventTrack::GetEndContext() const
    {
        if (false == _contextQueue.empty())
        {
            return _contextQueue.back();
        }
        return nullptr;
    }
    std::string EventTrack::CopyContext(EventContext* context)
    {
        if (nullptr != context)
        {
            std::string copyBuffer = context->GetEventType();
            copyBuffer += "\n";
            copyBuffer += context->SerializedReflectFields();
            return copyBuffer;
        }
        return "";
    }
    std::string EventTrack::CopyContextFromID(UINT id)
    {
        EventContext* context = GetContextFromID(id);
        return CopyContext(context);
    }
    bool EventTrack::PasteContext(std::string_view data, float time)
    {
        if (data.empty())
        {
            return false;
        }
        size_t pos = data.find('\n');
        if (pos != std::string::npos)
        {
            std::string   typeName    = std::string(data.substr(0, pos));
            std::string   serialData  = std::string(data.substr(pos + 1));
            EventContext* context     = AddEventExFromCopyBuffer(serialData, typeName, time);
            if (context)
            {
                return true;
            }
        }
        return false;
    }
    void EventTrack::Sort()
    {
        std::sort(_contextQueue.begin(), _contextQueue.end(), 
            [](const EventContext* a, const EventContext* b)
            { 
                return a->Time < b->Time;
            });
    }
    void EventTrack::SetMinFrame(float minFrame) 
    {
         ReflectFields->MinFrame = minFrame;
        _currFrame = ImClamp(_currFrame, ReflectFields->MinFrame, ReflectFields->MaxFrame);
    }
    void EventTrack::SetMaxFrame(float maxFrame)
    {
        ReflectFields->MaxFrame = maxFrame;
        _currFrame = ImClamp(_currFrame, ReflectFields->MinFrame, ReflectFields->MaxFrame);
    }
    void EventTrack::SetCurrentFrame(float frame, bool pass) 
    {
        _currFrame = frame < GetMinFrame() ? GetMinFrame() : frame;
        if (true == pass || false == IsActive())
        {
            _prevFrame = _currFrame;
        }
    }
    void EventTrack::SetOwnerGameObject(std::weak_ptr<GameObject> weakObj) 
    {
        for (auto& context : _contextQueue)
        {
            if (context)
            {
                context->SetGameObject(weakObj);
            }
        }
    }
    void EventTrack::RequestNotify(float startTime, float endTime)
    {
        if (startTime >= endTime || true == HasFlags(EVENT_TRCK_FLAGS_NOTIFY_DISABLED))
        {
            return;
        }

        auto comp = [](const EventContext* context, float time) { return context->Time < time; };

        auto beginIt = std::lower_bound(_contextQueue.begin(), _contextQueue.end(), startTime, comp);

        for (auto it = beginIt; it != _contextQueue.end(); ++it)
        {
            EventContext* context = (*it);
            if (context)
            {
                if (context->Time > endTime)
                {
                    break;
                }
                else
                {
                    bool canNotify = true;
                    if (_preNotifyCallback)
                    {
                        canNotify = _preNotifyCallback(context);
                    }
                    if (canNotify)
                    {
                        context->OnNotify();
                        if (_postNotifyCallback)
                        {
                            _postNotifyCallback(context);
                        }
                    }
                }
            }
        }
    }
    void EventTrack::SerializedReflectEvent() 
    {
        ReflectFields->SerializedDataList.clear();
        for (const auto& context : _contextQueue)
        {
            if (nullptr != context)
            {
                std::pair<EventTypeName, std::string> data;
                data = {context->GetEventType(), context->SerializedReflectFields()};
                ReflectFields->SerializedDataList.push_back(data);
            }
        }
    }
    void EventTrack::DeserializedReflectEvent() 
    {
        ClearContext();
        for (const auto& [typeName, data] : ReflectFields->SerializedDataList)
        {
            EventContext* context = NewInstanceWithKey(typeName);
            if (nullptr == context)
            {
                context = new EventContext();
            }
            context->DeserializedReflectFields(data);
            _contextQueue.push_back(context);
            _contextTable[context->ID] = context;
        }
        Sort();
    }
} // namespace Timeline