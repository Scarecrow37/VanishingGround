#pragma once
#include "EventContext/TimelineEventContext.h"

namespace Timeline
{
    enum Flag
    {
        TIMELINE_FLAG_NONE              = 0,
        TIMELINE_FLAG_USE_COUNTER       = 1 << 0, // 카운터 사용 여부. (매 틱 독자적으로 DeltaTime을 더한다.)
        TIMELINE_FLAG_USE_LOOP          = 1 << 2, // 루프 여부. (카운터 사용 시에 만)
        TIMELINE_FLAG_NOTIFY_DISABLED   = 1 << 3  // 알림 비활성화 여부. (NotifyEvent가 호출되지 않음.)
    };

    /// <summary>
    /// 저장된 타임 스탬프에 따라 이벤트를 호출하는 클래스입니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <typename CONTEXT>
    class EventTrack : public ReflectSerializer
    {
    public:
        EventTrack();
        virtual ~EventTrack();

    public:
        void Update();
        void Clear();
        void ClearContext();

    public:
        CONTEXT* AddEvent(std::string_view label, float time);
        CONTEXT* AddEventEx(std::string_view label, std::string_view typenameID, float time, UINT id = UINT_MAX);
        bool RemoveNotifyFromID(UINT id);
        bool     RemoveNotifyFromEvent(CONTEXT** event);
        bool RemoveNotifyFromNotify(EventContextBase** context);
        bool ChangeNotifyTime(UINT id, float time);
        bool ChangeNotifyEvent(UINT id, std::string_view typeNameID);
        CONTEXT* GetNotifyFromID(UINT id) const;

        void Sort();
        void SetMinFrame(float minFrame);
        void SetMaxFrame(float maxFrame);

        inline void SetFlags(TimeLineSystemFlags flags) { ReflectFields->Flags = flags; }
        inline void AddFlags(TimeLineSystemFlags flags) { ReflectFields->Flags |= flags; }
        inline void RemoveFlags(TimeLineSystemFlags flags) { ReflectFields->Flags &= ~flags; }
        inline void ToggleFlags(TimeLineSystemFlags flags) { ReflectFields->Flags ^= flags; }
        inline bool HasFlags(TimeLineSystemFlags flags) const { return ReflectFields->Flags & flags; }

        inline float GetCurrentFrame() const { return _currFrame; }
        inline float GetPreviousFrame() const { return _prevFrame; }
        inline float GetMaxFrame() const { return ReflectFields->MaxFrame; }
        inline float GetMinFrame() const { return ReflectFields->MinFrame; }
        inline bool  IsActive() const { return _isActie; }
        inline bool  IsPlaying() const { return _isPlaying; }
        inline bool  IsVaildFrame(float frame) const { return frame >= GetMinFrame() && frame <= GetMaxFrame(); }
        inline size_t GetEventContextCount() const { return _contextQueue.size(); }
        inline const std::vector<EventContextBase*>& GetEventContextQueue() const { return _contextQueue; }

    private:
        void ProcessNotifies(float startTime, float endTime);
        inline bool IsDirty() { _currFrame != _prevFrame; } 
        inline UINT GetUniqueID() { return ++ReflectFields->UniqueID; }

    private:
        float _currFrame;
        float _prevFrame;
        bool  _isActie;
        bool  _isPlaying;

        std::vector<EventContextBase*> _contextQueue; // 이벤트 큐
        std::unordered_map<UINT, EventContextBase*> _contextTable;

        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        float                    MinFrame = 0.0f;
        float                    MaxFrame = 0.0f;
        int                      Flags    = 0;
        UINT                     UniqueID = 0;
        std::vector<std::string> SerializedDataList;
        REFLECT_FIELDS_END(TimelineSystem)
    };

    template <typename EVENT>
    inline EventTrack<EVENT>::EventTrack()
    {
        Clear();
    }

    template <typename EVENT>
    inline EventTrack<EVENT>::~EventTrack()
    {
        ClearContext();
    }

    template <typename EVENT>
    inline void EventTrack<EVENT>::Update()
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
    template <typename EVENT>
    inline void EventTrack<EVENT>::Clear()
    {
        _currFrame = 0.0f;
        _prevFrame = 0.0f;
        _isActie   = true;
        _isPlaying = false;
        ClearContext();
    }
    template <typename EVENT>
    inline void EventTrack<EVENT>::ClearContext()
    {
        for (auto& notify : _contextQueue)
        {
            if (notify->Context)
            {
                delete notify->Context;
                notify->Context = nullptr;
            }
        }
    }

    template <typename EVENT>
    inline EVENT* EventTrack<EVENT>::AddEvent(std::string_view label, float time)
    {
        const char* key = typeid(EVENT).name();
        return AddEventEx(label, key, time);
    }

    template <typename EVENT>
    inline EVENT* EventTrack<EVENT>::AddEventEx(std::string_view label, std::string_view typenameID, float time, UINT id)
    {
        UINT uniqueID = (id == UINT_MAX) ? GetUniqueID() : id;
        auto it = _contextTable.find(uniqueID);
        if (it != _contextTable.end())
        {
            return it->second;
        }
        EventContext<EVENT>* context = new EventContext<EVENT>(uniqueID);
        context->SetEvent(typenameID);
        context->SetTime(time);
        context->Label = label;
        _contextQueue.push_back(context);
        _contextTable[uniqueID] = context;
        Sort();
        return context;
    }
    template <typename EVENT>
    inline bool EventTrack<EVENT>::RemoveNotifyFromID(UINT id)
    {
        for (auto it = _contextQueue.begin(); it != _contextQueue.end(); ++it)
        {
            if ((*it)->ID == id)
            {
                delete (*it);
                _contextQueue.erase(it);
                _contextTable.erase(id);
                return true;
            }
        }
        return false;
    }
    template <typename EVENT>
    inline bool EventTrack<EVENT>::RemoveNotifyFromEvent(EVENT** event)
    {
        for (auto it = _contextQueue.begin(); it != _contextQueue.end(); ++it)
        {
            if ((*it)->Event == (*event))
            {
                delete (*it);
                (*event) = nullptr;
                _contextQueue.erase(it);
                _contextTable.erase((*it)->ID);
                return true;
            }
        }
        return false;
    }
    template <typename EVENT>
    inline bool EventTrack<EVENT>::RemoveNotifyFromNotify(EventContextBase** context)
    {
        for (auto it = _contextQueue.begin(); it != _contextQueue.end(); ++it)
        {
            if ((*it) == (*context))
            {
                delete (*context);
                _contextQueue.erase(it);
                return true;
            }
        }
        return false;
    }
    template <typename EVENT>
    inline bool EventTrack<EVENT>::ChangeNotifyTime(UINT id, float time)
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
    template <typename EVENT>
    inline bool EventTrack<EVENT>::ChangeNotifyEvent(UINT id, std::string_view typeNameID)
    {
        TimelineNotify* notify = GetNotifyFromID(id);
        if (nullptr != notify)
        {
            notify->SetNotifyEvent(typeNameID);
            return true;
        }
        return false;
    }
    template <typename EVENT>
    inline EVENT* EventTrack<EVENT>::GetNotifyFromID(UINT id) const
    {
        auto it = _contextTable.find(id);
        if (it != _contextTable.end())
        {
            return it->second;
        }
        return nullptr;
    }
    template <typename EVENT>
    inline void EventTrack<EVENT>::Sort()
    {
        std::sort(_contextQueue.begin(), _contextQueue.end(), [](const EventContext<EVENT>* a, const EventContext<EVENT>* b)
        {
            return a->Time < b->Time;
        });
    }
    template <typename EVENT>
    inline void EventTrack<EVENT>::SetMinFrame(float minFrame)
    {
        ReflectFields->MinFrame = minFrame;
        _currFrame = ImClamp(_currFrame, GetMinFrame(), GetMaxFrame());
    }
    template <typename EVENT>
    inline void EventTrack<EVENT>::SetMaxFrame(float maxFrame)
    {
        ReflectFields->MaxFrame = maxFrame;
        _currFrame = ImClamp(_currFrame, GetMinFrame(), GetMaxFrame());
    }
    template <typename EVENT>
    inline void EventTrack<EVENT>::ProcessNotifies(float startTime, float endTime)
    {
        if (startTime >= endTime || true == HasFlags(TIMELINE_FLAG_NOTIFY_DISABLED))
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
}
