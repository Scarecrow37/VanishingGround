#pragma once

class TurnAction;

namespace Timeline
{
    /// <summary>
    /// 타임라인에 사용되는 이벤트 정보를 담고있는 객체입니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    class ActionContext 
        : public EventContextBase
        , public ReflectSerializer
    {
    public:
        USING_PROPERTY(ActionContext)
        ActionContext(UINT id = UINT_MAX);
        virtual ~ActionContext();

        GETTER_ONLY(TurnAction*, Event) { return _event; }
        PROPERTY(Event)

    private:
        virtual void SetEvent(std::string_view typeNameID) override;
        virtual void SetTime(float time) override;

    protected:
        TurnAction* _event = nullptr;

        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
    };
}
