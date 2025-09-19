#pragma once

namespace Timeline
{
    class EventContext : public ReflectSerializer
    {
        friend class EventTrack;
    public:
        USING_PROPERTY(EventContext)
        EventContext()          = default;
        virtual ~EventContext() = default;

        //REFLECT_PROPERTY(ID, Label, EventType, Time)
        REFLECT_PROPERTY()
        GETTER_ONLY(UINT, ID) { return ReflectFields->ContextID; }
        PROPERTY(ID)
        GETTER_ONLY(std::string_view, EventType) { return ReflectFields->EventType.c_str(); }
        PROPERTY(EventType)
        GETTER(std::string_view, Label) { return ReflectFields->Label; }
        SETTER(std::string_view, Label) { ReflectFields->Label = value; }
        PROPERTY(Label)
        GETTER_ONLY(float, Time) { return ReflectFields->Time; }
        PROPERTY(Time)

        inline const std::string& GetLabel()        const { return ReflectFields->Label; }
        inline const std::string& GetEventType()    const { return ReflectFields->EventType; }
        inline float              GetTime()         const { return ReflectFields->Time; }

        void SetEvent(std::string_view typeNameID);
        void SetTime(float time);
        void SetLabel(std::string_view label);
        bool IsValidID() const;
        bool IsSameEvent(const EventContext* other) const;

        /// <summary>
        /// 알림이 발생했을 때 호출되는 가상 함수입니다. 호출 시 동작할 동작을 오버라이딩하여 구현해야합니다.
        /// </summary>
        virtual void OnNotify() {}
        virtual void ImGuiDrawPropertysEvent() {}

    protected:
        /// <summary>
        /// typeID를 통해 콘텍스트를 생성해야하는 인터페이스 함수입니다.
        /// </summary>
        /// <param name="typeNameID">해당 이벤트의 typenameID</param>
        virtual void RequireEvent(std::string_view typeNameID) {};

    protected:
        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        UINT        ContextID       = UINT_MAX;
        float       Time            = 0.0f;
        std::string Label           = "";
        std::string EventType       = "";
        std::string SerializedData  = "";
        REFLECT_FIELDS_END(EventContext)
    };
} // namespace Timeline