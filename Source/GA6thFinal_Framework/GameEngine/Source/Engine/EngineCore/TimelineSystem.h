#pragma once

class ITimelineEvent : public ReflectSerializer
{
    USING_PROPERTY(ITimelineEvent)
public:
    REFLECT_PROPERTY()

    virtual void OnNotified(float time) = 0;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(ITimelineEvent)
};

class TimelineNotify : public ReflectSerializer
{
    USING_PROPERTY(TimelineNotify)
public:
    REFLECT_PROPERTY()
    TimelineNotify(float time, ITimelineEvent* event);
    TimelineNotify() = default;
    virtual ~TimelineNotify();

public:
    inline void Notify() { _event->OnNotified(ReflectFields->Time); }

    inline void  SetTime(float time) { ReflectFields->Time = time; }
    inline float GetTime() const { return ReflectFields->Time; }

    inline void            SetEvent(ITimelineEvent* event) { _event = event; }
    inline ITimelineEvent* GetEvent() const { return _event; }

protected:
    ITimelineEvent* _event = nullptr;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    float       Time = 0.0f;
    std::string EventName = "";
    REFLECT_FIELDS_END(TimelineNotify)
};

using TimeLineSystemFlags = int;

class TimelineSystem 
    : public ReflectSerializer, public FactoryConstructor<ITimelineEvent>
{
    using TimelineList = std::vector<TimelineNotify*>;
public:
    enum Flag
    {
        TIMELINESYSTEM_FLAGS_NONE            = 0,
        TIMELINESYSTEM_FLAGS_USE_COUNTER     = 1 << 0, // 카운터 사용 여부. (매 틱 독자적으로 DeltaTime을 더한다.)
        TIMELINESYSTEM_FLAGS_LOOP            = 1 << 2, // 루프 여부.
        TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED = 1 << 3  // 알림 비활성화 여부. (NotifyEvent가 호출되지 않음.)
    };

public:
    TimelineSystem();
    ~TimelineSystem();

public:
    template<typename T> 
    T* AddNotify(float time)
    {
        static_assert(std::is_base_of_v<ITimelineEvent, T>, "T is not derived from ITimelineEvent.");
        const char* key = typeid(T).name();
        ITimelineEvent* instance = NewInstanceWithKey(key);
        if (nullptr != instance)
        {
            TimelineNotify* notify = new TimelineNotify();
            notify->SetTime(time);
            notify->SetEvent(instance);
            _timelineNotifyQueue.push_back(notify);
            
            Sort();
            return static_cast<T*>(instance);
        }
        return nullptr;
    }

    bool RemoveNotifyFromEvent(ITimelineEvent* event);
    bool RemoveNotifyFromIndex(size_t index);

    bool ChangeNotifyTimeFromEvent(ITimelineEvent* event, float newTime);
    bool ChangeNotifyTimeFromIndex(size_t index, float newTime);

    TimelineNotify* GetNotifyFromIndex(size_t index) const;

public:
    void Update();

    void Play();
    void Stop();
    void Resume();
    void Pause();

    void ClearNotifies();

    void ResetFrame();

    void SetMinFrame(float minFrame);
    void SetMaxFrame(float maxFrame);

    void SetCurrentFrame(float frame, bool pass = false);

    /* Flags */
    inline void SetFlags(TimeLineSystemFlags flags) { _flags = flags; }
    inline void AddFlags(TimeLineSystemFlags flags) { _flags |= flags; }
    inline void RemoveFlags(TimeLineSystemFlags flags) { _flags &= ~flags; }
    inline void ToggleFlags(TimeLineSystemFlags flags) { _flags ^= flags; }
    inline bool HasFlags(TimeLineSystemFlags flags) const { return _flags & flags; }

    /* Getter */
    inline float    GetMaxFrame() const { return _maxFrame; }
    inline float    GetMinFrame() const { return _minFrame; }
    inline float    GetCurrentFrame() const { return _currFrame; }
    inline float    GetPreviousFrame() const { return _prevFrame; }
    inline bool     IsPlaying() const { return _isPlaying; }
    inline size_t   GetNotifyCount() const { return _timelineNotifyQueue.size(); }
    inline const TimelineList& GetTimelineNotifyList() const { return _timelineNotifyQueue; }

private:
    void ProcessNotifies(float startTime, float endTime);
    void Sort();
    static bool CompareNotifyToAsending(const TimelineNotify* a, const TimelineNotify* b);

private:
    float _minFrame;
    float _maxFrame;
    float _currFrame;
    float _prevFrame;
    bool  _isPlaying;
    int   _flags;
    TimelineList _timelineNotifyQueue;
};