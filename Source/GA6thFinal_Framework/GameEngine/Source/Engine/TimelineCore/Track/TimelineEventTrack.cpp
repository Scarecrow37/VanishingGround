#include "pch.h"
#include "TimelineEventTrack.h"

namespace Timeline
{
    EventTrack::EventTrack()
    {
        _currFrame = 0.0f;
        _prevFrame = 0.0f;
        _isActie   = true;
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
        _isActie   = true;
        _isPlaying = false;
    }
    EventContextBase* EventTrack::AddEvent(std::string_view label, float time)
    {
        return nullptr;
    }
    EventContextBase* EventTrack::AddEventEx(std::string_view label, std::string_view typenameID, float time, UINT id)
    {
        return nullptr;
    }
} // namespace Timeline