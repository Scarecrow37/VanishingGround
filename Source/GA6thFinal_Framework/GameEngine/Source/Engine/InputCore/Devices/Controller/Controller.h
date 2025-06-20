#pragma once
#include "../../Declare/InputError.h"

namespace Input
{
    class Adapter;

    /// <summary>
    /// 게임 컨트롤러의 상태를 관리하고 입력을 조회하는 클래스입니다.
    /// </summary>
    class Controller
    {
        /// <summary>
        /// 컨트로러 버튼의 타입에 대한 비트을 정의합니다.
        /// </summary>
        using Button                    = unsigned short;
        constexpr Button DPAD_UP        = 0x0001;
        constexpr Button DPAD_DOWN      = 0x0002;
        constexpr Button DPAD_LEFT      = 0x0004;
        constexpr Button DPAD_RIGHT     = 0x0008;
        constexpr Button START          = 0x0010;
        constexpr Button BACK           = 0x0020;
        constexpr Button LEFT_THUMB     = 0x0040;
        constexpr Button RIGHT_THUMB    = 0x0080;
        constexpr Button LEFT_SHOULDER  = 0x0100;
        constexpr Button RIGHT_SHOULDER = 0x0200;
        constexpr Button A              = 0x1000;
        constexpr Button B              = 0x2000;
        constexpr Button X              = 0x4000;
        constexpr Button Y              = 0x8000;

        /// <summary>
        /// 컨트롤러의 식별자입니다.
        /// </summary>
        using ID = unsigned char;

        /// <summary>
        /// 엄지 막대의 위치에 대한 값입니다.
        /// X축, Y축 모두 값의 범위는 -1.0 ~ 1.0 사이입니다.
        /// magnitude는 벡터의 크기를 나타내며, 0.0 ~ 1.0 사이의 값을 가집니다.
        /// </summary>
        struct ThumbStickAxis
        {
            float x;
            float y;
            float magnitude;
        };

        /// <summary>
        /// 컨트롤러의 Trigger의 누름 정도에 대한 값입니다.
        /// 값의 범위는 0.0 ~ 1.0 사이입니다.
        /// </summary>
        using TriggerValue = float;

        /// <summary>
        /// 컨트롤러의 각 버튼의 누름에 대한 신호 값입니다.
        /// 값의 범위는 <see cref="ControllerButton"/>를 참고하세요.
        /// </summary>
        using Buttons = Button;

        /// <summary>
        /// 컨트롤러의 현재 상태를 나타내는 구조체입니다.
        /// </summary>
        struct State
        {
            using Generation = DWORD;

            Generation     Generation;
            ThumbStickAxis LeftThumbStickAxis;
            ThumbStickAxis RightThumbStickAxis;
            TriggerValue   LeftTrigger;
            TriggerValue   RightTrigger;
            Buttons        Buttons;
        };

    public:
        Controller(const Adapter* adapter);

        Result Connect();

        bool IsConnected() const;

        Result UpdateState();

        ThumbStickAxis GetLeftThumbStickAxis() const;

        ThumbStickAxis GetRightThumbStickAxis() const;

        TriggerValue GetLeftTrigger() const;

        TriggerValue GetRightTrigger() const;

        bool IsButtonDown(Button button) const;

        bool IsButtonUp(Button button) const;

        ID GetID() const;
    private:
        const Adapter* _adapter;

        ID _id;
        State _state;
    };
} // namespace Input