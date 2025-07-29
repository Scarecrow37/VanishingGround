#pragma once

namespace Timeline
{
    class EventContext : public ReflectSerializer
    {
    public:
        USING_PROPERTY(EventContext)
        EventContext()          = default;
        virtual ~EventContext() = default;

        REFLECT_PROPERTY(ID, Label, EventName, Time)

        GETTER_ONLY(UINT, ID) { return ReflectFields->ContextID; }
        PROPERTY(ID)
        GETTER(std::string_view, Label) { return ReflectFields->Label; }
        SETTER(std::string_view, Label) { ReflectFields->Label = value; }
        PROPERTY(Label)
        GETTER_ONLY(std::string_view, EventName) { return ReflectFields->EventNameData.c_str(); }
        PROPERTY(EventName)
        GETTER_ONLY(float, Time) { return ReflectFields->TimeData; }
        PROPERTY(Time)

        const std::string& GetLabel()       const { return ReflectFields->Label; }
        const std::string& GetEventName()   const { return ReflectFields->EventNameData; }
        float              GetTime()        const { return ReflectFields->TimeData; }

    public:
        virtual void OnNotify() = 0;

        void SetEvent(std::string_view typeNameID);
        void SetTime(float time);
        bool IsValidID() const;
        bool IsSameEvent(const EventContext* other) const;

    protected:
        /// <summary>
        /// typeID를 통해 이벤트를 생성해야하는 인터페이스 함수입니다.
        /// </summary>
        /// <param name="typeNameID">해당 이벤트의 typenameID</param>
        virtual void RequireEvent(std::string_view typeNameID)  = 0;

    protected:
        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        UINT        ContextID      = UINT_MAX;
        float       TimeData       = 0.0f;
        std::string Label          = "";
        std::string EventNameData  = "";
        std::string SerializedData = "";
        REFLECT_FIELDS_END(EventContext)
    };
} // namespace Timeline