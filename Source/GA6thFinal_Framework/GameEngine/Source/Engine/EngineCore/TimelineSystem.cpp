#include "pch.h"
#include "TimelineSystem.h"

TimelineNotify::TimelineNotify()
{
}
TimelineNotify::~TimelineNotify() 
{
}

void TimelineNotify::Notify()
{
    _event->OnNotified(Time);
}

void TimelineNotify::SetNotifyEvent(float time, std::string_view typeNameID) 
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

bool TimelineSystem::RemoveNotifyFromEvent(ITimelineEvent* event)
{
    for (auto it = _timelineNotifyQueue.begin(); it != _timelineNotifyQueue.end(); ++it)
    {
        TimelineNotify* notify = (*it);
        if (notify->Event == event)
        {
            _timelineNotifyQueue.erase(it);
            return true;
        }
    }
    return false;
}

bool TimelineSystem::RemoveNotifyFromIndex(size_t index)
{
    if (index < 0 || index >= _timelineNotifyQueue.size())
    {
        return false;
    }
    auto it = _timelineNotifyQueue.begin() + index;
    _timelineNotifyQueue.erase(it);
    return true;
}

bool TimelineSystem::ChangeNotifyTimeFromEvent(ITimelineEvent* event, float newTime)
{
    for (auto& notify : _timelineNotifyQueue)
    {
        if (notify->Event == event)
        {
            notify->Time = newTime;
            Sort();
            return true;
        }
    }
    return false;
}

bool TimelineSystem::ChangeNotifyTimeFromIndex(size_t index, float newTime)
{
    if (index < 0 || index >= _timelineNotifyQueue.size())
    {
        return false;
    }
    _timelineNotifyQueue[index]->Time = newTime;
    Sort();
    return true;
}

TimelineNotify* TimelineSystem::GetNotifyFromIndex(size_t index) const
{
    if (index < 0 || index >= _timelineNotifyQueue.size())
    {
        throw std::out_of_range("Index out of range in TimelineSystem::GetNotifyFromIndex");
    }
    return _timelineNotifyQueue[index];
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
    }
    Sort();
}
