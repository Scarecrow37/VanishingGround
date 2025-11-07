#pragma once
#include "ControllerTypes.h"

namespace Input
{
    class Adapter;

    /// <summary>
    /// 게임 컨트롤러의 상태를 관리하고 입력을 조회하는 클래스입니다.
    /// </summary>
    class Controller
    {
    public:
        /// <summary>
        /// 컨트롤러의 식별자입니다.
        /// </summary>
        using ID = ControllerTypes::ID;

        /// <summary>
        /// 컨트롤러 버튼의 타입에 대한 비트을 정의합니다.
        /// </summary>
        using Button = ControllerTypes::Button;

        /// <summary>
        /// ControllerTypes 네임스페이스의 ButtonState 타입에 대한 별칭을 정의합니다.
        /// </summary>
        using ButtonState = ControllerTypes::ButtonState;

        /// <summary>
        /// ControllerTypes::ButtonQueue 타입에 대한 별칭을 ButtonQueue로 정의합니다.
        /// </summary>
        using ButtonQueue = ControllerTypes::ButtonQueue;

        /// <summary>
        /// 엄지 막대의 위치에 대한 값입니다.
        /// X축, Y축 모두 값의 범위는 -1.0 ~ 1.0 사이입니다.
        /// magnitude는 벡터의 크기를 나타내며, 0.0 ~ 1.0 사이의 값을 가집니다.
        /// </summary>
        using ThumbStickAxis = ControllerTypes::ThumbStickAxis;

        /// <summary>
        /// 엄지 막대의 편향에 대한 정보입니다.
        /// X축과, Y축의 값을 비교하여 상하좌우로 편향되었는지 나타냅니다.
        /// </summary>
        using StickBias = ControllerTypes::StickBias;

        /// <summary>
        /// 컨트롤러의 Trigger의 누름 정도에 대한 값입니다.
        /// 값의 범위는 0.0 ~ 1.0 사이입니다.
        /// </summary>
        using TriggerValue = ControllerTypes::TriggerValue;

        /// <summary>
        /// 컨트롤러의 현재 상태를 나타내는 구조체입니다.
        /// </summary>
        using State = ControllerTypes::State;

        /// <summary>
        /// ControllerTypes 네임스페이스의 StateFlag 타입에 대한 별칭을 정의합니다.
        /// </summary>
        using StateFlag = ControllerTypes::StateFlag;

        static constexpr std::pair<const char*, Button> BUTTON_FLAG_LIST[] = {
            {"DPAD_UP", Button::DPAD_UP},
            {"DPAD_DOWN", Button::DPAD_DOWN},
            {"DPAD_LEFT", Button::DPAD_LEFT},
            {"DPAD_RIGHT", Button::DPAD_RIGHT},
            {"START", Button::START},
            {"BACK", Button::BACK},
            {"LEFT_THUMB_BUTTON", Button::LEFT_THUMB_BUTTON},
            {"RIGHT_THUMB_BUTTON", Button::RIGHT_THUMB_BUTTON},
            {"LEFT_SHOULDER", Button::LEFT_SHOULDER},
            {"RIGHT_SHOULDER", Button::RIGHT_SHOULDER},
            {"A", Button::A},
            {"B", Button::B},
            {"X", Button::X},
            {"Y", Button::Y},
            {"LEFT_THUMB_STICK", Button::LEFT_THUMB_STICK},
            {"RIGHT_THUMB_STICK", Button::RIGHT_THUMB_STICK},
            {"LEFT_TRIGGER", Button::LEFT_TRIGGER},
            {"RIGHT_TRIGGER", Button::RIGHT_TRIGGER},
        };

    public:
        static constexpr const char* GetButtonName(const Button button) noexcept
        {
            switch (button)
            {
            case Button::UNDEFINED:
                return "UNDEFINED";
            case Button::DPAD_UP:
                return "DPAD_UP";
            case Button::DPAD_DOWN:
                return "DPAD_DOWN";
            case Button::DPAD_LEFT:
                return "DPAD_LEFT";
            case Button::DPAD_RIGHT:
                return "DPAD_RIGHT";
            case Button::START:
                return "START";
            case Button::BACK:
                return "BACK";
            case Button::LEFT_THUMB_BUTTON:
                return "LEFT_THUMB_BUTTON";
            case Button::RIGHT_THUMB_BUTTON:
                return "RIGHT_THUMB_BUTTON";
            case Button::LEFT_SHOULDER:
                return "LEFT_SHOULDER";
            case Button::RIGHT_SHOULDER:
                return "RIGHT_SHOULDER";
            case Button::A:
                return "A";
            case Button::B:
                return "B";
            case Button::X:
                return "X";
            case Button::Y:
                return "Y";
            case Button::LEFT_THUMB_STICK:
                return "LEFT_THUMB_STICK";
            case Button::RIGHT_THUMB_STICK:
                return "RIGHT_THUMB_STICK";
            case Button::LEFT_TRIGGER:
                return "LEFT_TRIGGER";
            case Button::RIGHT_TRIGGER:
                return "RIGHT_TRIGGER";
            }

            return "UNKNOWN_BUTTON";
        }
        static constexpr const char* GetStickBiasName(const StickBias bias) noexcept
        {
            switch (bias)
            {
            case StickBias::UNBIASED:
                return "UNBIASED";
            case StickBias::BIAS_UP:
                return "UP";
            case StickBias::BIAS_DOWN:
                return "DOWN";
            case StickBias::BIAS_LEFT:
                return "LEFT";
            case StickBias::BIAS_RIGHT:
                return "RIGHT";
            case StickBias::BIAS_UP_LEFT:
                return "UP_LEFT";
            case StickBias::BIAS_UP_RIGHT:
                return "UP_RIGHT";
            case StickBias::BIAS_DOWN_LEFT:
                return "DOWN_LEFT";
            case StickBias::BIAS_DOWN_RIGHT:
                return "DOWN_RIGHT";
            }
            return "UNKNOWN_BIAS";
        }

    public:
        explicit Controller(const Adapter* adapter);

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
        /// 왼쪽 스틱의 편향 값을 반환합니다.
        /// </summary>
        /// <returns>왼쪽 스틱의 편향을 나타내는 StickBias 값입니다.</returns>
        [[nodiscard]] StickBias GetLeftStickBias() const noexcept;

        /// <summary>
        /// 오른쪽 스틱의 편향 값을 반환합니다.
        /// </summary>
        /// <returns>오른쪽 스틱의 편향을 나타내는 StickBias 값입니다.</returns>
        [[nodiscard]] StickBias GetRightStickBias() const noexcept;

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
        [[nodiscard]] ButtonQueue GetButtonQueue() const noexcept;

        /// <summary>
        /// 컨트롤러의 왼쪽 및 오른쪽 모터를 진동시킵니다. 진동을 종료하고 싶으면 진동 속도를 0으로 설정하면 됩니다.
        /// </summary>
        /// <param name="leftMotorSpeed">왼쪽 모터의 진동 속도.</param>
        /// <param name="rightMotorSpeed">오른쪽 모터의 진동 속도.</param>
        void Vibrate(ControllerTypes::MotorSpeed leftMotorSpeed, ControllerTypes::MotorSpeed rightMotorSpeed) const;

        /// <summary>
        /// 컨트롤러에 지정된 진동을 적용합니다.
        /// </summary>
        /// <param name="vibration">적용할 진동 설정을 나타내는 ControllerTypes::Vibration 객체입니다.</param>
        void Vibrate(const ControllerTypes::Vibration& vibration);

    private:
        void        UpdateStickBias(const ButtonQueue& queue);
        void        UpdateVibration();
        ButtonQueue UpdateQueue(ButtonQueue& queue);
        void        UpdateQueueState(const ButtonQueue& queue);

        /// <summary>
        /// 컨트롤러의 진동을 지정된 모터 속도와 지속 시간으로 활성화합니다.
        /// </summary>
        /// <param name="leftMotorSpeed">왼쪽 모터의 진동 속도.</param>
        /// <param name="rightMotorSpeed">오른쪽 모터의 진동 속도.</param>
        /// <param name="duration">진동이 지속될 시간(밀리초 단위).</param>
        void Vibrate(ControllerTypes::MotorSpeed leftMotorSpeed, ControllerTypes::MotorSpeed rightMotorSpeed, std::chrono::milliseconds duration);

    private:
        const Adapter* _adapter;

        ID          _id;

        State       _state;

        ButtonQueue _queue;
        unsigned int _queueState;

        StickBias   _leftStickBias;
        StickBias    _rightStickBias;

        ControllerTypes::Vibration _nextVibration;
        std::mutex              _vibrationMutex;
        std::condition_variable _vibrationConditionVariable;
        std::future<void>       _vibrationFuture;
    };
} // namespace Input