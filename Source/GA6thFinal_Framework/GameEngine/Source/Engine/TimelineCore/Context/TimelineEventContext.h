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

        REFLECT_PROPERTY(ID, Label, EventType, Time)
        GETTER_ONLY(UINT, ID) { return ReflectFields->ContextID; }
        PROPERTY(ID)
        GETTER_ONLY(std::string_view, EventType) { return ReflectFields->EventType; }
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
        virtual void ImGuiDrawPropertysEvent() override {}

        /// <summary>
        /// EventContext를 지정된 타입으로 캐스팅합니다.
        /// </summary>
        /// <typeparam name="T">캐스팅할 대상 타입입니다.</typeparam>
        /// <param name="context">캐스팅할 EventContext 객체의 포인터입니다.</param>
        /// <returns>T 타입의 포인터로 캐스팅된 객체입니다. 캐스팅이 실패하면 nullptr을 반환할 수 있습니다.</returns>
        template <typename T>
        T* Cast(EventContext* context)
        {
            if (context)
            {
                const char* typeNameT = typeid(T).name();
                const auto& keyTypeName  = context->GetEventType();
                if (typeNameT == keyTypeName)
                {
                    return static_cast<T*>(context);
                }
            }
            return nullptr;
        }

        /// <summary>
        /// EventContext 포인터를 지정된 타입으로 안전하게 캐스팅합니다.
        /// </summary>
        /// <typeparam name="T">캐스팅할 대상 타입입니다.</typeparam>
        /// <param name="context">캐스팅할 EventContext 포인터입니다.</param>
        /// <returns>캐스팅이 성공하면 T 타입의 포인터를 반환하고, 실패하거나 context가 nullptr이면 nullptr를 반환합니다.</returns>
        template <typename T>
        T* SafeCast(EventContext* context)
        {
            if (context)
            {
                return dynamic_cast<T*>(context);
            }
            return nullptr;
        }

    protected:
        /// <summary>
        /// typeID를 통해 콘텍스트를 생성해야하는 가상 함수입니다.
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