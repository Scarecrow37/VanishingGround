#pragma once
#include "../../Declare/InputError.h"

namespace Input
{
    class ControllerAdapter;

    /// <summary>
    /// 게임 컨트롤러의 상태를 관리하고 입력을 조회하는 클래스입니다.
    /// </summary>
    class Controller
    {
    public:
        /// <summary>
        /// 컨트롤러의 식별자입니다.
        /// </summary>
        using ID                       = unsigned char;
        static constexpr ID MAX_CONNECTION_COUNT  = 4;
        static constexpr ID INVALID_ID = MAX_CONNECTION_COUNT;

        /// <summary>
        /// 컨트로러 버튼의 타입에 대한 비트을 정의합니다.
        /// </summary>
        using Button                           = unsigned short;
        static constexpr Button DPAD_UP        = 0x0001;
        static constexpr Button DPAD_DOWN      = 0x0002;
        static constexpr Button DPAD_LEFT      = 0x0004;
        static constexpr Button DPAD_RIGHT     = 0x0008;
        static constexpr Button START          = 0x0010;
        static constexpr Button BACK           = 0x0020;
        static constexpr Button LEFT_THUMB     = 0x0040;
        static constexpr Button RIGHT_THUMB    = 0x0080;
        static constexpr Button LEFT_SHOULDER  = 0x0100;
        static constexpr Button RIGHT_SHOULDER = 0x0200;
        static constexpr Button A              = 0x1000;
        static constexpr Button B              = 0x2000;
        static constexpr Button X              = 0x4000;
        static constexpr Button Y              = 0x8000;

        /// <summary>
        /// 엄지 막대의 위치에 대한 값입니다.
        /// X축, Y축 모두 값의 범위는 -1.0 ~ 1.0 사이입니다.
        /// magnitude는 벡터의 크기를 나타내며, 0.0 ~ 1.0 사이의 값을 가집니다.
        /// </summary>
        struct ThumbStickAxis
        {
            float X;
            float Y;
            float Magnitude;
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
            using Generation = unsigned long;

            Generation     StateGeneration;
            ThumbStickAxis LeftThumbStickAxis;
            ThumbStickAxis RightThumbStickAxis;
            TriggerValue   LeftTrigger;
            TriggerValue   RightTrigger;
            Buttons        Buttons;
        };

    public:
        explicit Controller(const ControllerAdapter* adapter);

        /// <summary>
        /// 컨트롤러를 연결합니다.
        /// 성능상의 이유로 매 프레임마다 호출하지 않고 몇 초 간격을 두고 호출하는 것이 좋습니다.
        /// </summary>
        /// <returns>연결에 성공하면 INPUT_ERROR_SUCCESS, 연결에 실패하면 INPUT_ERROR_NOT_CONNECTED가
        /// 반환됩니다.</returns>
        Result Connect();

        /// <summary>
        /// 연결되어 있는지 여부를 확인합니다.
        /// </summary>
        /// <returns>연결되어 있으면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsConnected() const;

        /// <summary>
        /// 상태를 갱신하고 결과를 반환합니다.
        /// </summary>
        /// <returns>상태 갱신 작업의 결과를 나타내는 Result 객체입니다.</returns>
        Result UpdateState();

        /// <summary>
        /// 왼쪽 엄지스틱의 축 값을 반환합니다.
        /// </summary>
        /// <returns>왼쪽 엄지스틱의 현재 축 값을 나타내는 ThumbStickAxis 객체입니다.</returns>
        [[nodiscard]] ThumbStickAxis GetLeftThumbStickAxis() const;

        /// <summary>
        /// 오른쪽 엄지스틱의 축 값을 반환합니다.
        /// </summary>
        /// <returns>오른쪽 엄지스틱의 축 값을 나타내는 ThumbStickAxis 객체를 반환합니다.</returns>
        [[nodiscard]] ThumbStickAxis GetRightThumbStickAxis() const;

        /// <summary>
        /// 왼쪽 트리거의 값을 반환합니다.
        /// </summary>
        /// <returns>왼쪽 트리거의 현재 값을 나타내는 TriggerValue 객체입니다.</returns>
        [[nodiscard]] TriggerValue GetLeftTrigger() const;

        /// <summary>
        /// 오른쪽 트리거의 값을 반환합니다.
        /// </summary>
        /// <returns>오른쪽 트리거의 현재 값을 나타내는 TriggerValue 객체입니다.</returns>
        [[nodiscard]] TriggerValue GetRightTrigger() const;

        /// <summary>
        /// 버튼이 눌려 있는지 여부를 확인합니다.
        /// </summary>
        /// <param name="button">상태를 확인할 버튼입니다.</param>
        /// <returns>버튼이 눌려 있으면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsButtonDown(Button button) const;

        /// <summary>
        /// 버튼이 눌려 있지 않은지 확인합니다.
        /// </summary>
        /// <param name="button">상태를 확인할 버튼입니다.</param>
        /// <returns>버튼이 올라가 있으면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsButtonUp(Button button) const;

        /// <summary>
        /// 컨트롤러의 ID를 반환합니다.
        /// </summary>
        /// <returns>컨트롤러의 ID를 반환합니다.</returns>
        [[nodiscard]] ID GetID() const;

    private:
        const ControllerAdapter* _adapter;

        ID    _id;
        State _state;
    };
} // namespace Input