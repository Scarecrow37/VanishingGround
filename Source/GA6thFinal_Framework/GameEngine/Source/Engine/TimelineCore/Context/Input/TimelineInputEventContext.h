#pragma once

namespace Timeline
{
    class InputEventContext : public EventContext
    {
        enum EventMode
        {
            EVENT_VIBRATION,
        };

    public:
        USING_PROPERTY(InputEventContext)
        InputEventContext();
        ~InputEventContext() override;

    public:
        REFLECT_PROPERTY() 

        void OnNotify() override;

        void SerializedReflectEvent() override;
        void DeserializedReflectEvent() override;
        void ImGuiDrawPropertysEvent() override;

    private:
        void Vibrate();
        void UpdateVibrateState();
        void UpdateVibrateState(const Input::ControllerTypes::Vibration& vibration);

    private:
        Input::ControllerTypes::Vibration _vibrationState;

        REFLECT_FIELDS_BEGIN(EventContext)
        int   Event           = EVENT_VIBRATION;
        int   Duration        = 1000;
        float LeftMotorSpeed  = 0.5f;
        float RightMotorSpeed = 0.5f;
        REFLECT_FIELDS_END(InputEventContext)
    };
}
