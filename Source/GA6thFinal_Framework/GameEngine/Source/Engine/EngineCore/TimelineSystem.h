#pragma once

/// <summary>
/// TimelineEvent의 인터페이스입니다.
/// TimelineEvent는 TimelineNotify에 의해 특정 시간에 호출되는 이벤트입니다.
/// </summary>
class ITimelineEvent : public ReflectSerializer
{
    USING_PROPERTY(ITimelineEvent)
public:
    REFLECT_PROPERTY()

    virtual void OnNotified(float time) = 0;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(ITimelineEvent)
};

/// <summary>
/// TimelineEvent를 특정 시간에 맞춰 호출해주는 객체입니다.
/// </summary>
class TimelineNotify : public ReflectSerializer
{
    USING_PROPERTY(TimelineNotify)
public:
    TimelineNotify(UINT id = UINT_MAX);
    virtual ~TimelineNotify();

public:
    void Notify();
    void SetNotifyEventAndTime(std::string_view typeNameID, float time);
    void SetNotifyEvent(std::string_view typeNameID);
    void SetNotifyTime(float time);
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
    /// <summary>
    /// Notify를 추가합니다.
    /// </summary>
    /// <typeparam name="T">추가할 TimelineEvent 클래스</typeparam>
    /// <param name="time">TimelineEvent가 호출 될 시간 값</param>
    /// <returns>추가한 Notify의 포인터</returns>
    template<typename T> 
    TimelineNotify* AddNotify(float time)
    {
        static_assert(std::is_base_of_v<ITimelineEvent, T>, "T is not derived from ITimelineEvent.");
        const char* key = typeid(T).name();
        return AddNotify(time, key);
    }

    /// <summary>
    /// TypeName을 통해 Notify를 추가합니다.
    /// </summary>
    /// <param name="time">TimelineEvent가 호출 될 시간 값</param>
    /// <param name="typenameID">TimelineEvent 객체의 TypeName 값</param>
    /// <param name="id">Notify가 등록될 id입니다. default 인자로 호출 시 기본 값이 등록됩니다.</param>
    /// <returns>이미 추가된 id라면 기존 Notify의 포인터를, 추가된 적 없는 id라면 새로 추가한 Notify의 포인터를 반환합니다.</returns>
    TimelineNotify* AddNotify(std::string_view typenameID, float time, UINT id = UINT_MAX)
    {
        UINT uniqueID = (id == UINT_MAX) ? GetUniqueID() : id;
        auto it = _idToNotifyTable.find(uniqueID);
        if (it != _idToNotifyTable.end())
        {
            return it->second;
        }
        TimelineNotify* notify   = new TimelineNotify(uniqueID);
        notify->SetNotifyEventAndTime(typenameID, time);
        _timelineNotifyQueue.push_back(notify);
        _idToNotifyTable[uniqueID] = notify;
        Sort();
        return notify;
    }

    /// <summary>
    /// ID를 통해 Notify를 제거합니다.
    /// </summary>
    /// <param name="id">제거할 Notify의 ID값</param>
    /// <returns>
    /// <para>제거에 성공할 시 true를 반환합니다.</para>
    /// <para>해당 인자를 통해 객체를 찾지 못하면 false를 반환합니다.</para>
    /// </returns>
    bool RemoveNotifyFromID(UINT id);
    /// <summary>
    /// ITimelineEvent를 통해 Notify를 제거합니다.
    /// 제거에 성공하면, 인자로 넣은 ITimelineEvent포인터의 포인터를 nullptr로 설정합니다.
    /// </summary>
    /// <param name="event">제거할 Notify의 ITimelineEvent 이중 포인터</param>
    /// <returns>
    /// <para>제거에 성공할 시 true를 반환합니다.</para>
    /// <para>해당 인자를 통해 객체를 찾지 못하면 false를 반환합니다.</para>
    /// </returns>
    bool RemoveNotifyFromEvent(ITimelineEvent** event);
    /// <summary>
    /// TimelineNotify를 통해 Notify를 제거합니다.
    /// 제거에 성공하면, 인자로 넣은 TimelineNotify포인터의 포인터를 nullptr로 설정합니다.
    /// </summary>
    /// <param name="event">제거할 Notify의 TimelineNotify 이중 포인터</param>
    /// <returns>
    /// <para>제거에 성공할 시 true를 반환합니다.</para>
    /// <para>해당 인자를 통해 객체를 찾지 못하면 false를 반환합니다.</para>
    /// </returns>
    bool RemoveNotifyFromNotify(TimelineNotify** notify);

    /// <summary>
    /// Notify의 시간을 변경합니다.
    /// </summary>
    /// <param name="id">변경할 Notify의 ID값</param>
    /// <param name="time">변경할 시간</param>
    /// <returns></returns>
    bool ChangeNotifyTime(UINT id, float time);
    /// <summary>
    /// Notify의 이벤트를 변경합니다.
    /// </summary>
    /// <param name="id">변경할 Notify의 ID값</param>
    /// <param name="typeNameID">변경할 이벤트 객체의 TypeNameID</param>
    /// <returns></returns>
    bool ChangeNotifyEvent(UINT id, std::string_view typeNameID);

    /// <summary>
    /// ID로부터 Notify를 찾습니다.
    /// </summary>
    /// <param name="id">찾을 Notify의 ID값</param>
    /// <returns>
    /// <para>찾는데 성공하면 해당 ID값의 TimelineNotify의 포인터를 반환합니다.</para>
    /// <para>실패하면 nullptr을 반환합니다.</para>
    /// </returns>
    TimelineNotify* GetNotifyFromID(UINT id) const;

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

    /*/// Flags ///*/
    inline void SetFlags(TimeLineSystemFlags flags) { ReflectFields->Flags = flags; }
    inline void AddFlags(TimeLineSystemFlags flags) { ReflectFields->Flags |= flags; }
    inline void RemoveFlags(TimeLineSystemFlags flags) { ReflectFields->Flags &= ~flags; }
    inline void ToggleFlags(TimeLineSystemFlags flags) { ReflectFields->Flags ^= flags; }
    inline bool HasFlags(TimeLineSystemFlags flags) const { return ReflectFields->Flags & flags; }

    /*/// Get ///*/
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