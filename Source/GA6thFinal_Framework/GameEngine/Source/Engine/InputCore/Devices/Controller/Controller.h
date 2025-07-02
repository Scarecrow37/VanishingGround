#pragma once

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
        using ID                                 = unsigned char;
        static constexpr ID MAX_CONNECTION_COUNT = 4;
        static constexpr ID INVALID_ID           = MAX_CONNECTION_COUNT;

        /// <summary>
        /// 컨트롤러 버튼의 타입에 대한 비트을 정의합니다.
        /// </summary>
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
            A                  = 0x01000,
            B                  = 0x02000,
            X                  = 0x04000,
            Y                  = 0x08000,
            LEFT_THUMB_STICK   = 0x10000,
            RIGHT_THUMB_STICK  = 0x20000,
            LEFT_TRIGGER       = 0x40000,
            RIGHT_TRIGGER      = 0x80000,
        };

        /// <summary>
        /// 컨트롤러 버튼의 플래그 목록입니다.
        ///</summary>
        static constexpr std::pair<const char*, Button> BUTTON_FLAG_LIST[] = {
            {"DPAD_UP", DPAD_UP},
            {"DPAD_DOWN", DPAD_DOWN},
            {"DPAD_LEFT", DPAD_LEFT},
            {"DPAD_RIGHT", DPAD_RIGHT},
            {"START", START},
            {"BACK", BACK},
            {"LEFT_THUMB_BUTTON", LEFT_THUMB_BUTTON},
            {"RIGHT_THUMB_BUTTON", RIGHT_THUMB_BUTTON},
            {"LEFT_SHOULDER", LEFT_SHOULDER},
            {"RIGHT_SHOULDER", RIGHT_SHOULDER},
            {"A", A},
            {"B", B},
            {"X", X},
            {"Y", Y},
            {"LEFT_THUMB_STICK", LEFT_THUMB_STICK},
            {"RIGHT_THUMB_STICK", RIGHT_THUMB_STICK},
            {"LEFT_TRIGGER", LEFT_TRIGGER},
            {"RIGHT_TRIGGER", RIGHT_TRIGGER},
        };

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
            Button         Buttons;
        };

    public:
        explicit Controller(const ControllerAdapter* adapter);

        /// <summary>
        /// 컨트롤러를 연결합니다.
        /// 성능상의 이유로 매 프레임마다 호출하지 않고 몇 초 간격을 두고 호출하는 것이 좋습니다.
        /// </summary>
        /// <exception cref="DeviceNotConnectedException">연결에 실패한 경우 발생합니다.</exception>
        void Connect();

        /// <summary>
        /// 연결되어 있는지 여부를 확인합니다.
        /// </summary>
        /// <returns>연결되어 있으면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsConnected() const noexcept;

        /// <summary>
        /// 상태를 갱신하고 결과를 반환합니다.
        /// </summary>
        /// <exception cref="DeviceNotConnectedException">컨트롤러가 연결되어 있지 않은 경우 발생합니다.</exception>
        /// <exception cref="InputException">상태 갱신에 실패한 경우 발생합니다.</exception>
        void UpdateState();

        /// <summary>
        /// 왼쪽 엄지스틱의 축 값을 반환합니다.
        /// </summary>
        /// <returns>왼쪽 엄지스틱의 현재 축 값을 나타내는 ThumbStickAxis 객체입니다.</returns>
        [[nodiscard]] ThumbStickAxis GetLeftThumbStickAxis() const noexcept;

        /// <summary>
        /// 오른쪽 엄지스틱의 축 값을 반환합니다.
        /// </summary>
        /// <returns>오른쪽 엄지스틱의 축 값을 나타내는 ThumbStickAxis 객체를 반환합니다.</returns>
        [[nodiscard]] ThumbStickAxis GetRightThumbStickAxis() const noexcept;

        /// <summary>
        /// 왼쪽 트리거의 값을 반환합니다.
        /// </summary>
        /// <returns>왼쪽 트리거의 현재 값을 나타내는 TriggerValue 객체입니다.</returns>
        [[nodiscard]] TriggerValue GetLeftTrigger() const noexcept;

        /// <summary>
        /// 오른쪽 트리거의 값을 반환합니다.
        /// </summary>
        /// <returns>오른쪽 트리거의 현재 값을 나타내는 TriggerValue 객체입니다.</returns>
        [[nodiscard]] TriggerValue GetRightTrigger() const noexcept;

        /// <summary>
        /// 버튼이 눌려 있는지 여부를 확인합니다.
        /// </summary>
        /// <param name="button">상태를 확인할 버튼입니다.</param>
        /// <returns>버튼이 눌려 있으면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsButtonDown(Button button) const noexcept;

        /// <summary>
        /// 버튼이 눌려 있지 않은지 확인합니다.
        /// </summary>
        /// <param name="button">상태를 확인할 버튼입니다.</param>
        /// <returns>버튼이 올라가 있으면 true, 그렇지 않으면 false를 반환합니다.</returns>
        [[nodiscard]] bool IsButtonUp(Button button) const noexcept;

        /// <summary>
        /// 컨트롤러의 ID를 반환합니다.
        /// </summary>
        /// <returns>컨트롤러의 ID를 반환합니다.</returns>
        [[nodiscard]] ID GetID() const noexcept;

        /// <summary>
        /// 버튼 큐를 반환합니다. 큐는 매 업데이트마다 갱신됩니다.
        /// </summary>
        /// <returns>저장된 Button 객체들의 std::queue를 반환합니다.</returns>
        [[nodiscard]] std::queue<Button> GetButtonQueue() const noexcept;

    private:
        const ControllerAdapter* _adapter;

        ID    _id;
        State _state;
        std::queue<Button> _queue;
    };
} // namespace Input