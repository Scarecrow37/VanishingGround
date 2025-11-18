#pragma once


namespace Timeline
{
    class EventContext;
    enum Flag
    {
        EVENT_TRACK_FLAGS_NONE            = 0,
        EVENT_TRACK_FLAGS_USE_COUNTER     = 1 << 0, // 카운터 사용 여부. (매 틱 독자적으로 DeltaTime을 더한다.)
        EVENT_TRACK_FLAGS_LOOP            = 1 << 2, // 루프 여부. (카운터 사용 시에 만)
        EVENT_TRACK_FLAGS_NOTIFY_DISABLED = 1 << 3  // 알림 비활성화 여부. (ContextEvent가 호출되지 않음.)
    };
    using EventTrackFlags = int;

    class EventTrack : public ReflectSerializer, public FactoryConstructor<EventContext>
    {
        using ContextQueue  = std::vector<EventContext*>;
        using ContextTable  = std::unordered_map<UINT, EventContext*>;
        using EventTypeName = std::string;

    public:
        EventTrack();
        virtual ~EventTrack();

    public:
        void Update();
        void Clear();
        void ClearContext();
        void SetDefault();

        void SetActive(bool active);
        void Play();
        void Stop();
        void Resume();
        void Pause();

    public:
        /// <summary>
        /// 새 이벤트를 추가하고 해당 이벤트의 컨텍스트를 반환합니다.
        /// </summary>
        /// <param name="label">이벤트를 식별하는 문자열입니다.</param>
        /// <param name="time">이벤트가 발생하는 시간입니다. 기본 값을 넣을 경우 현재 프레임에 이벤트를 추가합니다.</param>
        /// <returns>추가된 이벤트의 EventContext 포인터를 반환합니다.</returns>
        template <typename T> requires std::is_base_of_v<EventContext, T>
        EventContext*   AddEvent(std::string_view label, float time = FLT_MIN);
        EventContext*   AddEventFromTypeName(std::string_view label, std::string_view typenameID, float time, UINT id = UINT_MAX);
        EventContext*   AddEventFromCopyBuffer(std::string_view serialData, std::string_view typenameID, float time, UINT id = UINT_MAX);

        /// <summary>
        /// ID에서 컨텍스트를 제거합니다.
        /// </summary>
        /// <param name="id">컨텍스트를 제거할 대상의 식별자입니다.</param>
        /// <returns>컨텍스트가 성공적으로 제거되면 true를 반환하고, 그렇지 않으면 false를 반환합니다.</returns>
        bool RemoveContextFromID(UINT id);

        /// <summary>
        /// 지정된 ID의 컨텍스트 시간을 변경합니다.
        /// </summary>
        /// <param name="id">컨텍스트를 식별하는 UINT 형식의 ID입니다.</param>
        /// <param name="time">새로 설정할 시간(초 단위)입니다.</param>
        /// <returns>컨텍스트 시간 변경이 성공하면 true, 실패하면 false를 반환합니다.</returns>
        bool ChangeContextTime(UINT id, float time);

        /// <summary>
        /// 지정된 ID와 타입 이름 ID를 사용하여 컨텍스트 이벤트를 변경합니다.
        /// </summary>
        /// <param name="id">컨텍스트 이벤트를 식별하는 UINT 형식의 ID입니다.</param>
        /// <param name="typeNameID">이벤트 타입 이름을 나타내는 std::string_view 형식의 식별자입니다.</param>
        /// <returns>컨텍스트 이벤트 변경이 성공하면 true, 실패하면 false를 반환합니다.</returns>
        bool ChangeContextEvent(UINT id, std::string_view typeNameID);

        /// <summary>ID 값을 사용하여 EventContext 포인터를 반환합니다.</summary>
        /// <returns>id에 해당하는 EventContext 객체의 포인터입니다. 해당하는 컨텍스트가 없으면 nullptr을 반환할 수 있습니다.</returns>
        EventContext* GetContextFromID(UINT id) const;

        /// <summary>주어진 라벨에 해당하는 EventContext 포인터를 반환합니다.</summary>
        /// <returns>라벨에 해당하는 EventContext 객체의 포인터를 반환합니다. 해당하는 컨텍스트가 없으면 nullptr을 반환할 수 있습니다.</returns>
        EventContext* GetContextFromLabel(std::string_view label) const;

        /// <summary>ID에 해당하는 다음 순서의 이벤트 컨텍스트를 반환합니다.</summary>
        /// <returns>다음 순서의 이벤트 컨텍스트 객체에 대한 포인터입니다. 해당하는 컨텍스트가 없으면 nullptr을 반환할 수 있습니다.</returns>
        EventContext* GetNextContextFromID(UINT id) const;

        /// <summary>ID 값을 사용하여 이전 순서의 이벤트 컨텍스트를 반환합니다.</summary>
        /// <returns>이전 순서의 이벤트 컨텍스트 객체에 대한 포인터입니다. 해당하는 컨텍스트가 없으면 nullptr을 반환할 수 있습니다.</returns>
        EventContext* GetPrevContextFromID(UINT id) const;

        /// <summary>가장 첫 번째 해당하는 이벤트 컨텍스트를 반환합니다.</summary>
        /// <returns>첫 번째 이벤트 컨텍스트의 포인터입니다. 트랙에 아무 컨텍스트도 존재하지 않으면 nullptr을 반환할 수 있습니다.</returns>
        EventContext* GetBeginContext() const;

        /// <summary>가장 마지막에 해당하는 이벤트 컨텍스트를 반환합니다.</summary>
        /// <returns>마지막에 이벤트 컨텍스트의 포인터입니다. 트랙에 아무 컨텍스트도 존재하지 않으면 nullptr을 반환할 수 있습니다.</returns>
        EventContext* GetEndContext() const;

        /// <summary>
        /// EventContext 객체의 내용을 복사하여 컨텍스트 데이터 문자열을 생성합니다.
        /// </summary>
        /// <param name="context">복사할 EventContext 객체에 대한 포인터입니다.</param>
        /// <returns>복사된 컨텍스트 정보를 포함하는 std::string 객체입니다.</returns>
        std::string CopyContext(EventContext* context) const;
        std::string CopyContextFromID(UINT id) const;

        /// <summary>
        /// 지정된 데이터와 시간으로 컨텍스트를 붙여넣습니다.
        /// </summary>
        /// <param name="data">붙여넣을 컨텍스트 데이터(문자열 뷰).</param>
        /// <param name="time">컨텍스트를 붙여넣을 시간(기본값: FLT_MIN = 현재 프레임).</param>
        /// <returns>붙여넣기 성공 시 true, 실패 시 false를 반환합니다.</returns>
        bool PasteContext(std::string_view data, float time = FLT_MIN);

        void            Sort();
        void            SetMinFrame(float minFrame);
        void            SetMaxFrame(float maxFrame);
        void            SetCurrentFrame(float frame, bool donNotify = false);
        void            SetOwnerGameObject(std::weak_ptr<GameObject> weakObj);

        inline void     SetFlags(EventTrackFlags flags) { ReflectFields->Flags = flags; }
        inline void     AddFlags(EventTrackFlags flags) { ReflectFields->Flags |= flags; }
        inline void     RemoveFlags(EventTrackFlags flags) { ReflectFields->Flags &= ~flags; }
        inline void     ToggleFlags(EventTrackFlags flags) { ReflectFields->Flags ^= flags; }
        inline bool     HasFlags(EventTrackFlags flags) const { return ReflectFields->Flags & flags; }

        inline float    GetCurrentFrame() const { return _currFrame; }
        inline float    GetPreviousFrame() const { return _prevFrame; }
        inline float    GetMaxFrame() const { return ReflectFields->MaxFrame; }
        inline float    GetMinFrame() const { return ReflectFields->MinFrame; }
        inline bool     IsActive() const { return _isActive; }
        inline bool     IsPlaying() const { return _isPlaying; }
        inline bool     IsValidFrame(float frame) const { return frame >= GetMinFrame() && frame <= GetMaxFrame(); }
        inline bool     IsDirty() const { return _currFrame != _prevFrame; }
        inline size_t   GetEventContextCount() const { return _contextQueue.size(); }
        inline const std::vector<EventContext*>& GetEventContextQueue() const { return _contextQueue; }

        inline void     SetPreNotifyCallback(std::function<bool(const EventContext*)> callback) { _preNotifyCallback = callback; }
        inline void     SetPostNotifyCallback(std::function<void(const EventContext*)> callback) { _postNotifyCallback = callback; }

    private:
        void            RequestNotify(float startTime, float endTime);
        inline UINT     GetUniqueID() { return ++ReflectFields->UniqueID; }

    private:
        std::weak_ptr<GameObject> _gameObject; // 이벤트를 실행할 게임 오브젝트. (필요에 의해 초기화합니다.)

        float _currFrame;
        float _prevFrame;
        bool  _isActive;
        bool  _isPlaying;

        ContextQueue _contextQueue; 
        ContextTable _contextTable;

        std::function<bool(const EventContext*)> _preNotifyCallback;    // Notify Callback Function
        std::function<void(const EventContext*)> _postNotifyCallback;   // Notify Callback Function

        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        float                    MinFrame = 0.0f;
        float                    MaxFrame = 0.0f;
        EventTrackFlags          Flags    = EVENT_TRACK_FLAGS_NONE;
        UINT                     UniqueID = 0;
        std::vector<std::pair<EventTypeName, std::string>> SerializedDataList;
        REFLECT_FIELDS_END(EventTrack)

        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
    };

    template <typename T> requires std::is_base_of_v<EventContext, T>
    inline EventContext* EventTrack::AddEvent(std::string_view label, float time)
    {
        const char* key = typeid(T).name();
        return AddEventFromTypeName(label, key, time);
    }
} // namespace Timeline