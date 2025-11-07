#pragma once

namespace Input::ControllerTypes
{
    using ID                                 = unsigned char;
    static constexpr ID MAX_CONNECTION_COUNT = 4;
    static constexpr ID INVALID_ID           = MAX_CONNECTION_COUNT;

    enum Button : unsigned int
    {
        UNDEFINED          = 0x00000,
        DPAD_UP            = 0x00001,
        DPAD_DOWN          = 0x00002,
        DPAD_LEFT          = 0x00004,
        DPAD_RIGHT         = 0x00008,
        START              = 0x00010,
        BACK               = 0x00020,
        LEFT_THUMB_BUTTON  = 0x00040,
        RIGHT_THUMB_BUTTON = 0x00080,
        LEFT_SHOULDER      = 0x00100,
        RIGHT_SHOULDER     = 0x00200,
        LEFT_TRIGGER       = 0x00400,
        RIGHT_TRIGGER      = 0x00800,
        A                  = 0x01000,
        B                  = 0x02000,
        X                  = 0x04000,
        Y                  = 0x08000,
        LEFT_THUMB_STICK   = 0x10000,
        RIGHT_THUMB_STICK  = 0x20000,
    };

    enum StateFlag : unsigned char
    {
        STATE_DOWN,
        STATE_UP,
        STATE_REPEAT
    };

    struct ThumbStickAxis
    {
        float X;
        float Y;
        float Magnitude;
    };

    enum StickBias : unsigned char
    {
        UNBIASED = 0,
        BIAS_UP,
        BIAS_DOWN,
        BIAS_RIGHT,
        BIAS_LEFT,
        BIAS_UP_LEFT,
        BIAS_UP_RIGHT,
        BIAS_DOWN_RIGHT,
        BIAS_DOWN_LEFT
    };

    using TriggerValue = float;

    struct State
    {
        using Generation = unsigned long;

        Generation     StateGeneration;
        ThumbStickAxis LeftThumbStickAxis;
        ThumbStickAxis RightThumbStickAxis;
        TriggerValue   LeftTrigger;
        TriggerValue   RightTrigger;
        Button         Buttons;
    };

    struct ButtonState
    {
        Button    Button;
        StickBias Bias;
        StateFlag Flag;
    };

    using ButtonQueue = std::vector<ButtonState>;

    using MotorSpeed = unsigned short;

    struct Vibration
    {
        MotorSpeed LeftMotorSpeed;
        MotorSpeed RightMotorSpeed;
        std::chrono::milliseconds Duration;

        static Vibration GetFromNormalizeData(float leftMotorSpeed, float rightMotorSpeed, int duration)
        {
            Vibration vibration;
            vibration.LeftMotorSpeed  = (MotorSpeed)(std::clamp(leftMotorSpeed, 0.0f, 1.0f) * 65535.0f);
            vibration.RightMotorSpeed = (MotorSpeed)(std::clamp(rightMotorSpeed, 0.0f, 1.0f) * 65535.0f);
            vibration.Duration        = std::chrono::milliseconds(duration);
            return vibration;
        }
    };

    

    static constexpr Vibration VIBRATION_EMPTY{
        .LeftMotorSpeed = 0, .RightMotorSpeed = 0, .Duration = std::chrono::milliseconds(0)};
    static constexpr Vibration VIBRATION_VEHICLE_REVVING{
        .LeftMotorSpeed = 22000, .RightMotorSpeed = 7000, .Duration = std::chrono::milliseconds(300)};
    static constexpr Vibration VIBRATION_EXPLOSION{
        .LeftMotorSpeed = 60000, .RightMotorSpeed = 32000, .Duration = std::chrono::milliseconds(600)};
    static constexpr Vibration VIBRATION_GUN_SHOT{
        .LeftMotorSpeed = 15000, .RightMotorSpeed = 45000, .Duration = std::chrono::milliseconds(110)};
    static constexpr Vibration VIBRATION_BUTTON_CLICK{
        .LeftMotorSpeed = 1500, .RightMotorSpeed = 4000, .Duration = std::chrono::milliseconds(70)};
    static constexpr Vibration VIBRATION_SWORD_BASH{
        .LeftMotorSpeed = 35000, .RightMotorSpeed = 23000, .Duration = std::chrono::milliseconds(200)};
    static constexpr Vibration VIBRATION_DAGGER_STAB{
        .LeftMotorSpeed = 18000, .RightMotorSpeed = 17000, .Duration = std::chrono::milliseconds(120)};
    static constexpr Vibration VIBRATION_HAMMER_SMASH{
        .LeftMotorSpeed = 55000, .RightMotorSpeed = 29000, .Duration = std::chrono::milliseconds(350)};
    static constexpr Vibration VIBRATION_TAKE_DAMAGE{
        .LeftMotorSpeed = 30000, .RightMotorSpeed = 17000, .Duration = std::chrono::milliseconds(150)};
} // namespace Input::ControllerTypes