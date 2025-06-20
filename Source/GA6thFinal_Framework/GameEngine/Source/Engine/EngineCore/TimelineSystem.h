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
    TimelineNotify(UINT id = UINT_MAX);
    virtual ~TimelineNotify();

public:
    void Notify();
    void SetNotifyEvent(float time, std::string_view typeNameID);
    bool IsValidID() const;

    REFLECT_PROPERTY(EventName, Time, ID)

    GETTER_ONLY(std::string_view, EventName) { return ReflectFields->EventNameData.c_str(); }
    PROPERTY(EventName)

    GETTER(float, Time) { return ReflectFields->TimeData; }
    SETTER(float, Time) { ReflectFields->TimeData = value; }
    PROPERTY(Time)

    GETTER_ONLY(ITimelineEvent*, Event) { return _event; }
    PROPERTY(Event)

    GETTER_ONLY(int, ID) { return ReflectFields->NotifyID; }
    PROPERTY(ID)

protected:
    ITimelineEvent* _event = nullptr;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    float       TimeData = 0.0f;
    UINT        NotifyID = 0;
    std::string EventNameData = "";
    std::string SerializedData = "";
    REFLECT_FIELDS_END(TimelineNotify)

    virtual void SerializedReflectEvent() override;
    virtual void DeserializedReflectEvent() override;
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
        TIMELINESYSTEM_FLAGS_LOOP            = 1 << 2, // 루프 여부. (카운터 사용 시에 만)
        TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED = 1 << 3  // 알림 비활성화 여부. (NotifyEvent가 호출되지 않음.)
    };

public:
    TimelineSystem();
    ~TimelineSystem();

public:
    template<typename T> 
    TimelineNotify* AddNotify(float time)
    {
        static_assert(std::is_base_of_v<ITimelineEvent, T>, "T is not derived from ITimelineEvent.");
        const char* key = typeid(T).name();
        return AddNotify(time, key);
    }

    TimelineNotify* AddNotify(float time, std::string_view typenameID, UINT id = UINT_MAX)
    {
        UINT uniqueID = (id == UINT_MAX) ? GetUniqueID() : id;
        auto it = _idToNotifyTable.find(uniqueID);
        if (it != _idToNotifyTable.end())
        {
            return it->second;
        }
        TimelineNotify* notify   = new TimelineNotify(uniqueID);
        notify->SetNotifyEvent(time, typenameID);
        _timelineNotifyQueue.push_back(notify);
        _idToNotifyTable[uniqueID] = notify;
        Sort();
        return notify;
    }

    bool RemoveNotifyFromID(UINT id);
    bool RemoveNotifyFromEvent(ITimelineEvent** event);
    bool RemoveNotifyFromNotify(TimelineNotify** notify);
    bool RemoveNotifyFromIndex(size_t index);

    bool ChangeNotifyTimeFromEvent(ITimelineEvent* event, float newTime);
    bool ChangeNotifyTimeFromIndex(size_t index, float newTime);

    TimelineNotify* GetNotifyFromID(UINT id) const;
    TimelineNotify* GetNotifyFromIndex(size_t index) const;

public:
    void Update();

    void SetActive(bool active);
    void Play();
    void Stop();
    void Resume();
    void Pause();

    void ClearNotifies();

    void SetMinFrame(float minFrame);
    void SetMaxFrame(float maxFrame);

    void SetCurrentFrame(float frame, bool pass = false);

    /* Flags */
    inline void SetFlags(TimeLineSystemFlags flags) { ReflectFields->Flags = flags; }
    inline void AddFlags(TimeLineSystemFlags flags) { ReflectFields->Flags |= flags; }
    inline void RemoveFlags(TimeLineSystemFlags flags) { ReflectFields->Flags &= ~flags; }
    inline void ToggleFlags(TimeLineSystemFlags flags) { ReflectFields->Flags ^= flags; }
    inline bool HasFlags(TimeLineSystemFlags flags) const { return ReflectFields->Flags & flags; }

    /* Getter */
    inline float    GetMaxFrame() const { return ReflectFields->MaxFrame; }
    inline float    GetMinFrame() const { return ReflectFields->MinFrame; }
    inline float    GetCurrentFrame() const { return _currFrame; }
    inline float    GetPreviousFrame() const { return _prevFrame; }
    inline bool     IsActive() const { return _isActie; }
    inline bool     IsPlaying() const { return _isPlaying; }
    inline size_t   GetNotifyCount() const { return _timelineNotifyQueue.size(); }
    inline bool     IsVaildFrame(float frame) const { return frame >= GetMinFrame() && frame <= GetMaxFrame(); }
    inline const TimelineList& GetTimelineNotifyList() const { return _timelineNotifyQueue; }
    inline UINT     GetUniqueID() { return ++ReflectFields->UniqueID; }

private:
    bool IsDirty();
    void ProcessNotifies(float startTime, float endTime);
    void Sort();
    static bool CompareNotifyToAsending(const TimelineNotify* a, const TimelineNotify* b);

private:
    float _currFrame;
    float _prevFrame;
    bool  _isActie;
    bool  _isPlaying;
    TimelineList _timelineNotifyQueue;
    std::unordered_map<UINT, TimelineNotify*> _idToNotifyTable;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    float MinFrame  = 0.0f;
    float MaxFrame  = 0.0f;
    int   Flags     = 0;
    UINT  UniqueID  = 0;
    std::vector<std::string> SerializedDataList;
    REFLECT_FIELDS_END(TimelineSystem)

private:
    virtual void SerializedReflectEvent() override;
    virtual void DeserializedReflectEvent() override;
};