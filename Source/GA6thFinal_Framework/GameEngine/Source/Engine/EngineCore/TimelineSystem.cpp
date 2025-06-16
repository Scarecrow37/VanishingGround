#include "pch.h"
#include "TimelineSystem.h"

TimelineSystem::TimelineSystem() 
    : _maxFrame(0.0f)
    , _currFrame(0.0f)
    , _prevFrame(0.0f)
    , _isActive(true)
    , _isLoop(false)
    , _isNotifyEnabled(true)
{
}

TimelineSystem::~TimelineSystem() 
{
    ClearNotifies();
}

void TimelineSystem::Update()
{
    if (true == _isActive)
    {
        _prevFrame = _currFrame;
        _currFrame += UmTime.DeltaTime();
        if (_currFrame >= _maxFrame)
        {
            if (true == _isLoop)
            {
                _currFrame = _currFrame - _maxFrame;
                // overflow에 대한 처리
                ProcessNotifies(_prevFrame, _maxFrame);
                ProcessNotifies(0.0f, _currFrame);
            }
            else
            {
                _currFrame = _maxFrame;
            }
        }
        else
        {
            ProcessNotifies(_prevFrame, _currFrame);
        }
    }
}

void TimelineSystem::SetActive(bool isActive)
{
    _isActive = isActive;
}

void TimelineSystem::SetLoop(bool isLoop) 
{
    _isLoop = isLoop;
}

void TimelineSystem::SetNotifyEnabled(bool isEnabled) 
{
    _isNotifyEnabled = isEnabled;
}

void TimelineSystem::ClearNotifies()
{
    _timelineNotifyQueue.clear();
    _timelineNotifyTable.clear();
}

void TimelineSystem::ResetFrame() 
{
    _currFrame = 0.0f;
    _prevFrame = 0.0f;
}

void TimelineSystem::SetMaxFrame(float maxFrame) 
{
    _maxFrame = maxFrame;
    _currFrame = 0.0f;
    _prevFrame = 0.0f;
}

void TimelineSystem::SetCurrentFrame(float frame)
{
    _currFrame = frame;
    _prevFrame = frame;
}

void TimelineSystem::AddNotify(const char* name, float time, const TimelineNotify::Event& event) 
{
    _timelineNotifyQueue.emplace_back(name, time, event);
    _timelineNotifyTable[name] = &_timelineNotifyQueue.back();
    std::sort(_timelineNotifyQueue.begin(), _timelineNotifyQueue.end(),
              [](const TimelineNotify& a, const TimelineNotify& b) { return a.Time < b.Time; });
}

bool TimelineSystem::RemoveNotify(const char* name)
{
    auto it = _timelineNotifyTable.find(name);
    if (it != _timelineNotifyTable.end())
    {
        auto notifyIt = std::find_if(_timelineNotifyQueue.begin(), _timelineNotifyQueue.end(),
                                     [&name](const TimelineNotify& notify) { return notify.Name == name; });
        if (notifyIt != _timelineNotifyQueue.end())
        {
            _timelineNotifyQueue.erase(notifyIt);
            _timelineNotifyTable.erase(it);
            return true;
        }
    }
    return false;
}

void TimelineSystem::ProcessNotifies(float startTime, float endTime)
{
    if (startTime == endTime || false == _isNotifyEnabled)
    {
        return;
    }

    auto comp = [](const TimelineNotify& notify, float time) { return notify.Time < time; };

    auto beginIt = std::lower_bound(_timelineNotifyQueue.begin(), _timelineNotifyQueue.end(), startTime, comp);

    for (auto it = beginIt; it != _timelineNotifyQueue.end(); ++it)
    {
        if (it->Time > endTime)
        {
            break;
        }
        else if (it->NotifyEvent)
        {
            it->NotifyEvent();
        }
    }
}