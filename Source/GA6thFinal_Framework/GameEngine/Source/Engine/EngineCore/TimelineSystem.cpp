#include "pch.h"
#include "TimelineSystem.h"

TimelineNotify::TimelineNotify(float time, ITimelineEvent* event)
{
    ReflectFields->Time      = time;
    ReflectFields->EventName = typeid(*event).name();
    _event = event;
}
TimelineNotify::~TimelineNotify() 
{
}

TimelineSystem::TimelineSystem() 
    : _maxFrame(0.0f)
    , _currFrame(0.0f)
    , _prevFrame(0.0f)
    , _isPlaying(true)
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
        if (notify->GetEvent() == event)
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
        if (notify->GetEvent() == event)
        {
            notify->SetTime(newTime);
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
    _timelineNotifyQueue[index]->SetTime(newTime);
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
    if (true == IsPlaying())
    {
        _prevFrame = _currFrame;
        if (true == HasFlags(TIMELINESYSTEM_FLAGS_USE_COUNTER))
        {
            _currFrame += UmTime.DeltaTime();
        }
        if (_currFrame >= _maxFrame)
        {
            if (true == HasFlags(TIMELINESYSTEM_FLAGS_LOOP))
            {
                _currFrame = _currFrame - _maxFrame;
                // overflow에 대한 처리
                ProcessNotifies(_prevFrame, _maxFrame);
                ProcessNotifies(0.0f, _currFrame);
            }
            else
            {
                _currFrame = _maxFrame;
                ProcessNotifies(_prevFrame, _maxFrame);
                Stop();
            }
        }
        else
        {
            ProcessNotifies(_prevFrame, _currFrame);
        }
    }
}

void TimelineSystem::Play()
{
    _isPlaying = true;
    SetCurrentFrame(0.0f, true);
}

void TimelineSystem::Stop() 
{
    _isPlaying = false;
    SetCurrentFrame(0.0f, true);
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

void TimelineSystem::ResetFrame() 
{
    _currFrame = 0.0f;
    _prevFrame = 0.0f;
}

void TimelineSystem::SetMaxFrame(float maxFrame) 
{
    if (maxFrame < 0.0f)
    {
        return;
    }
    _maxFrame = maxFrame;
    _currFrame = ImClamp(_currFrame, 0.0f, _maxFrame);
}

void TimelineSystem::SetCurrentFrame(float frame, bool pass/* = false*/)
{
    _currFrame = std::min(frame, 0.0f); // 최대치는 넘어도 된다. 업데이트때 처리하기 때문
    if (true == pass || _prevFrame > _currFrame)
    {
        _currFrame = _prevFrame;
    }
}

void TimelineSystem::ProcessNotifies(float startTime, float endTime)
{
    if (startTime == endTime || true == HasFlags(TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED))
    {
        return;
    }

    auto comp = [](const TimelineNotify* notify, float time) { return notify->GetTime() < time; };

    auto beginIt = std::lower_bound(_timelineNotifyQueue.begin(), _timelineNotifyQueue.end(), startTime, comp);

    for (auto it = beginIt; it != _timelineNotifyQueue.end(); ++it)
    {
        TimelineNotify* notify = (*it);
        if (notify->GetTime() > endTime)
        {
            break;
        }
        else if (nullptr != notify->GetEvent())
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
    return a->GetTime() < b->GetTime();
}