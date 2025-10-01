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
        inline std::weak_ptr<GameObject> GetGameObject() const { return _gameObject; }

        void SetGameObject(std::weak_ptr<GameObject> obj);
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
        template <typename T>
        T* Cast(EventContext* context)
        {
            if (context)
            {
                const char* typeNameT = typeid(T).name();
                const auto& keyTypeName  = context->GetEventType();
                if (keyTypeName == typeNameT)
                {
                    return static_cast<T*>(context);
                }
            }
            return nullptr;
        }

        /// <summary>
        /// EventContext 포인터를 지정된 타입으로 dynamic_cast를 시도합니다.
        /// </summary>
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
        std::weak_ptr<GameObject> _gameObject;

        REFLECT_FIELDS_BEGIN(ReflectSerializer)
        UINT        ContextID       = UINT_MAX;
        float       Time            = 0.0f;
        std::string Label           = "";
        std::string EventType       = "";
        std::string SerializedData  = "";
        REFLECT_FIELDS_END(EventContext)
    };
} // namespace Timeline