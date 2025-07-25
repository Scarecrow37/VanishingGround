#pragma once

class EventContextBase;
namespace Timeline
{
    class EventTrack : public ReflectSerializer
    {
    public:
        EventTrack();
        virtual ~EventTrack();

    public:
        void Update();
        void Clear();
        void ClearContext();
        void SetDefault();

    public:
        EventContextBase* AddEvent(std::string_view label, float time);
        EventContextBase* AddEventEx(std::string_view label, std::string_view typenameID, float time,
                                     UINT id = UINT_MAX);
        bool              RemoveNotifyFromID(UINT id);
        bool              RemoveNotifyFromEvent(EventContextBase** event);
        bool              RemoveNotifyFromNotify(EventContextBase** context);
        bool              ChangeNotifyTime(UINT id, float time);
        bool              ChangeNotifyEvent(UINT id, std::string_view typeNameID);
        EventContextBase* GetNotifyFromID(UINT id) const;

        void Sort();
        void SetMinFrame(float minFrame);
        void SetMaxFrame(float maxFrame);
        void SetCurrentFrame(float frame, bool pass = false);

        inline void SetFlags(TimeLineSystemFlags flags) { ReflectFields->Flags = flags; }
        inline void AddFlags(TimeLineSystemFlags flags) { ReflectFields->Flags |= flags; }
        inline void RemoveFlags(TimeLineSystemFlags flags) { ReflectFields->Flags &= ~flags; }
        inline void ToggleFlags(TimeLineSystemFlags flags) { ReflectFields->Flags ^= flags; }
        inline bool HasFlags(TimeLineSystemFlags flags) const { return ReflectFields->Flags & flags; }

        inline float  GetCurrentFrame() const { return _currFrame; }
        inline float  GetPreviousFrame() const { return _prevFrame; }
        inline float  GetMaxFrame() const { return ReflectFields->MaxFrame; }
        inline float  GetMinFrame() const { return ReflectFields->MinFrame; }
        inline bool   IsActive() const { return _isActie; }
        inline bool   IsPlaying() const { return _isPlaying; }
        inline bool   IsVaildFrame(float frame) const { return frame >= GetMinFrame() && frame <= GetMaxFrame(); }
        inline size_t GetEventContextCount() const { return _contextQueue.size(); }
        inline const std::vector<EventContextBase*>& GetEventContextQueue() const { return _contextQueue; }

    private:
        void        ProcessNotifies(float startTime, float endTime);
        inline bool IsDirty() { _currFrame != _prevFrame; }
        inline UINT GetUniqueID() { return ++ReflectFields->UniqueID; }

    private:
        float _currFrame;
        float _prevFrame;
        bool  _isActie;
        bool  _isPlaying;

        std::vector<EventContextBase*>              _contextQueue; // 이벤트 큐
        std::unordered_map<UINT, EventContextBase*> _contextTable;

        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        float                    MinFrame = 0.0f;
        float                    MaxFrame = 0.0f;
        int                      Flags    = 0;
        UINT                     UniqueID = 0;
        std::vector<std::string> SerializedDataList;
        REFLECT_FIELDS_END(TimelineSystem)
    };
} // namespace Timeline