#pragma once

namespace Input::ControllerTypes
{
    using ID                                 = unsigned char;
    static constexpr ID MAX_CONNECTION_COUNT = 4;
    static constexpr ID INVALID_ID           = MAX_CONNECTION_COUNT;

    enum Button : unsigned int
    {
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

    struct ThumbStickAxis
    {
        float X;
        float Y;
        float Magnitude;
    };

    enum StickBias : unsigned char
    {
        UNBIASED,
        LEFT,
        RIGHT,
        UP,
        DOWN
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
} // namespace Input::ControllerTypes