#pragma once

class EventContext;
namespace Timeline
{
    enum Flag
    {
        EVENT_TRCK_FLAGS_NONE            = 0,
        EVENT_TRCK_FLAGS_USE_COUNTER     = 1 << 0, // 카운터 사용 여부. (매 틱 독자적으로 DeltaTime을 더한다.)
        EVENT_TRCK_FLAGS_LOOP            = 1 << 2, // 루프 여부. (카운터 사용 시에 만)
        EVENT_TRCK_FLAGS_NOTIFY_DISABLED = 1 << 3  // 알림 비활성화 여부. (ContextEvent가 호출되지 않음.)
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
        template <typename T> 
        EventContext* AddEvent(std::string_view label, float time)
        {
            static_assert(std::is_base_of_v<EventContext, T>, "T is not derived from ITimelineEvent.");
            const char* key = typeid(T).name();
            return AddEventEx(label, key, time);
        }
        EventContext*   AddEventEx(std::string_view label, std::string_view typenameID, float time, UINT id = UINT_MAX);
        bool            RemoveContext(EventContext** context);
        bool            RemoveContextFromID(UINT id);
        bool            ChangeContextTime(UINT id, float time);
        bool            ChangeContextEvent(UINT id, std::string_view typeNameID);
        EventContext*   GetContextFromID(UINT id) const;

        void            Sort();
        void            SetMinFrame(float minFrame);
        void            SetMaxFrame(float maxFrame);
        void            SetCurrentFrame(float frame, bool pass = false);

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
        inline bool     IsDirty() { return _currFrame != _prevFrame; }
        inline size_t   GetEventContextCount() const { return _contextQueue.size(); }
        inline const std::vector<EventContext*>& GetEventContextQueue() const { return _contextQueue; }

    private:
        void            RequestNotify(float startTime, float endTime);
        inline UINT     GetUniqueID() { return ++ReflectFields->UniqueID; }

    private:
        float _currFrame;
        float _prevFrame;
        bool  _isActive;
        bool  _isPlaying;

        ContextQueue _contextQueue; 
        ContextTable _contextTable;

        std::function<bool(const EventContext*)> _preNotifyCallback; // Notify Callback Function
        std::function<bool(const EventContext*)> _postNotifyCallback; // Notify Callback Function

        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        float                    MinFrame = 0.0f;
        float                    MaxFrame = 0.0f;
        EventTrackFlags          Flags    = 0;
        UINT                     UniqueID = 0;
        std::vector <std::pair<EventTypeName, std::string>> SerializedDataList;
        REFLECT_FIELDS_END(EventTrack)

        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
    };
} // namespace Timeline