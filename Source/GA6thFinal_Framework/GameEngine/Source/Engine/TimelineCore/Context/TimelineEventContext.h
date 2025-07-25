#pragma once

namespace Timeline
{
    class EventContextBase : public ReflectSerializer
    {
    public:
        USING_PROPERTY(EventContextBase)
        EventContextBase(UINT id = UINT_MAX);
        virtual ~EventContextBase();

        REFLECT_PROPERTY(ID, Label, EventName, Time)

        GETTER_ONLY(UINT, ID) { return ReflectFields->NotifyID; }
        PROPERTY(ID)
        GETTER(std::string_view, Label) { return ReflectFields->Label; }
        SETTER(std::string_view, Label) { ReflectFields->Label = value; }
        PROPERTY(Label)
        GETTER_ONLY(std::string_view, EventName) { return ReflectFields->EventNameData.c_str(); }
        PROPERTY(EventName)
        GETTER_ONLY(float, Time) { return ReflectFields->TimeData; }
        PROPERTY(Time)

    public:
        virtual void SetEvent(std::string_view typeNameID) = 0;
        virtual void SetTime(float time)                   = 0;

    public:
        bool IsValidID() const;

    protected:
        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        UINT        NotifyID       = 0;
        float       TimeData       = 0.0f;
        std::string Label          = "";
        std::string EventNameData  = "";
        std::string SerializedData = "";
        REFLECT_FIELDS_END(EventContextBase)
    };
} // namespace Timeline