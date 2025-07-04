#include "pch.h"
#include "TimelineSystem.h"

TimelineNotify::TimelineNotify(UINT id) 
{
    _event                        = nullptr;
    ReflectFields->TimeData       = 0.0f;
    ReflectFields->NotifyID       = id;
    ReflectFields->EventNameData  = "";
    ReflectFields->SerializedData = "";
}
TimelineNotify::~TimelineNotify()
{
    if (nullptr != _event)
    {
        delete _event;
        _event = nullptr;
    }
}

void TimelineNotify::Notify()
{
    _event->OnNotified(Time);
}

void TimelineNotify::SetNotifyEventAndTime(std::string_view typeNameID, float time)
{
    if (nullptr != _event)
    {
        delete _event;
        _event = nullptr;
    }

    ReflectFields->TimeData      = time;
    ReflectFields->EventNameData = typeNameID;

    _event = FactoryConstructor<ITimelineEvent>::NewInstanceWithKey(ReflectFields->EventNameData);
}

void TimelineNotify::SetNotifyEvent(std::string_view typeNameID)
{
    if (nullptr != _event)
    {
        delete _event;
        _event = nullptr;
    }
    ReflectFields->EventNameData = typeNameID;
    _event = FactoryConstructor<ITimelineEvent>::NewInstanceWithKey(ReflectFields->EventNameData);
}

void TimelineNotify::SetNotifyTime(float time)
{
    ReflectFields->TimeData = time;
}

bool TimelineNotify::IsValidID() const
{
    return ReflectFields->NotifyID != UINT_MAX;
}

void TimelineNotify::SerializedReflectEvent() 
{
    if (nullptr != _event)
    {
        ReflectFields->SerializedData = _event->SerializedReflectFields();
    }
}

void TimelineNotify::DeserializedReflectEvent() 
{
    _event = FactoryConstructor<ITimelineEvent>::NewInstanceWithKey(ReflectFields->EventNameData);
    if (nullptr != _event)
    {
        _event->DeserializedReflectFields(ReflectFields->SerializedData);
    }
}

TimelineSystem::TimelineSystem() 
    : _currFrame(0.0f)
    , _prevFrame(0.0f)
    , _isActie(true)
    , _isPlaying(false)
{
}

TimelineSystem::~TimelineSystem() 
{
    ClearNotifies();
}

bool TimelineSystem::RemoveNotifyFromID(UINT id)
{
    for (auto it = _timelineNotifyQueue.begin(); it != _timelineNotifyQueue.end(); ++it)
    {
        if ((*it)->ID == id)
        {
            delete (*it);
            _timelineNotifyQueue.erase(it);
            _idToNotifyTable.erase(id);
            return true;
        }
    }
    return false;
}

bool TimelineSystem::RemoveNotifyFromEvent(ITimelineEvent** event)
{
    for (auto it = _timelineNotifyQueue.begin(); it != _timelineNotifyQueue.end(); ++it)
    {
        if ((*it)->Event == (*event))
        {
            delete (*it);
            (*event) = nullptr;
            _timelineNotifyQueue.erase(it);
            _idToNotifyTable.erase((*it)->ID);
            return true;
        }
    }
    return false;
}

bool TimelineSystem::RemoveNotifyFromNotify(TimelineNotify** notify)
{
    for (auto it = _timelineNotifyQueue.begin(); it != _timelineNotifyQueue.end(); ++it)
    {
        if ((*it) == (*notify))
        {
            delete (*notify);
            _timelineNotifyQueue.erase(it);
            return true;
        }
    }
    return false;
}

bool TimelineSystem::ChangeNotifyTime(UINT id, float time)
{
    TimelineNotify* notify = GetNotifyFromID(id);
    if (nullptr != notify)
    {
        notify->SetNotifyTime(time);
        Sort();
        return true;
    }
    return false;
}

bool TimelineSystem::ChangeNotifyEvent(UINT id, std::string_view typeNameID)
{
    TimelineNotify* notify = GetNotifyFromID(id);
    if (nullptr != notify)
    {
        notify->SetNotifyEvent(typeNameID);
        return true;
    }
    return false;
}



TimelineNotify* TimelineSystem::GetNotifyFromID(UINT id) const
{
    auto it = _idToNotifyTable.find(id);
    if (it != _idToNotifyTable.end())
    {
        return it->second;
    }
    return nullptr;
}

void TimelineSystem::Update()
{
    if (GetMaxFrame() < GetMinFrame())
    {
        return;
    }
    if (true == IsActive())
    {
        if (true == IsPlaying() && true == HasFlags(TIMELINESYSTEM_FLAGS_USE_COUNTER))
        {
            SetCurrentFrame(_currFrame + UmTime.DeltaTime());
        }
        if (true == IsDirty())
        {
            // 이전 프레임이 현재 프레임보다 높다면 최대 값을 넘어가서 돌아왔다고 판단.
            if (_prevFrame > _currFrame)
            {
                ProcessNotifies(_prevFrame, GetMaxFrame());
                ProcessNotifies(GetMinFrame(), _currFrame);
            }
            else
            {
                ProcessNotifies(_prevFrame, _currFrame);
            }
        }
        _prevFrame = _currFrame;
        if (true == IsPlaying() && true == HasFlags(TIMELINESYSTEM_FLAGS_USE_COUNTER))
        {
            if (_currFrame >= GetMaxFrame())
            {
                if (true == HasFlags(TIMELINESYSTEM_FLAGS_LOOP))
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

void TimelineSystem::SetActive(bool active) 
{
    _isActie = active;
}

void TimelineSystem::Play()
{
    _isPlaying = true;
    SetCurrentFrame(GetMinFrame(), true);
}

void TimelineSystem::Stop() 
{
    _isPlaying = false;
    SetCurrentFrame(GetMinFrame(), true);
}

void TimelineSystem::Resume() 
{
    _isPlaying = true;
}

void TimelineSystem::Pause() 
{
    _isPlaying = false;
}

void TimelineSystem::ClearNotifies()
{
    _timelineNotifyQueue.clear();
}

void TimelineSystem::SetMinFrame(float minFrame) 
{
    ReflectFields->MinFrame = minFrame;
    _currFrame = ImClamp(_currFrame, GetMinFrame(), GetMaxFrame());
}

void TimelineSystem::SetMaxFrame(float maxFrame)
{
    ReflectFields->MaxFrame = maxFrame;
    _currFrame = ImClamp(_currFrame, GetMinFrame(), GetMaxFrame());
}

void TimelineSystem::SetCurrentFrame(float frame, bool pass/* = false*/)
{
    _currFrame = frame < GetMinFrame() ? GetMinFrame() : frame;
    if (true == pass || false == IsActive())
    {
        _prevFrame = _currFrame;
    }
}

bool TimelineSystem::IsDirty()
{
    return _currFrame != _prevFrame;
}

void TimelineSystem::ProcessNotifies(float startTime, float endTime)
{
    if (startTime >= endTime || true == HasFlags(TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED))
    {
        return;
    }

    auto comp = [](const TimelineNotify* notify, float time) { return notify->Time < time; };

    auto beginIt = std::lower_bound(_timelineNotifyQueue.begin(), _timelineNotifyQueue.end(), startTime, comp);

    for (auto it = beginIt; it != _timelineNotifyQueue.end(); ++it)
    {
        TimelineNotify* notify = (*it);
        if (notify->Time > endTime)
        {
            break;
        }
        else if (nullptr != notify->Event)
        {
            notify->Notify();
        }
    }
}

void TimelineSystem::Sort() 
{
    std::sort(_timelineNotifyQueue.begin(), _timelineNotifyQueue.end(), CompareNotifyToAsending);
}

bool TimelineSystem::CompareNotifyToAsending(const TimelineNotify* a, const TimelineNotify* b)
{
    return a->Time < b->Time;
}

void TimelineSystem::SerializedReflectEvent() 
{
    ReflectFields->SerializedDataList.clear();
    for (const auto& notify : _timelineNotifyQueue)
    {
        if (nullptr != notify)
        {
            ReflectFields->SerializedDataList.push_back(notify->SerializedReflectFields());
        }
    }
}

void TimelineSystem::DeserializedReflectEvent() 
{
    for (const auto& data : ReflectFields->SerializedDataList)
    {
        TimelineNotify* notify = new TimelineNotify();
        notify->DeserializedReflectFields(data);
        _timelineNotifyQueue.push_back(notify);
        _idToNotifyTable[notify->ID] = notify;
    }
    Sort();
}
