#pragma once
#include "ControllerTypes.h"

namespace Input
{
    /// <summary>
    /// XInputAdapter는 XInput을 사용하여 컨트롤러의 연결 및 상태 수신을 위한 인터페이스를 제공합니다.
    /// </summary>
    class Adapter
    {
    public:
        Adapter() = default;
        [[nodiscard]] ControllerTypes::ID          Connect() const noexcept;
        [[nodiscard]] ControllerTypes::State       ReceiveState(ControllerTypes::ID id) const;
        [[nodiscard]] ControllerTypes::ButtonQueue ReceiveQueue(ControllerTypes::ID id) const;
        void                                       SetVibration(ControllerTypes::ID id, ControllerTypes::MotorSpeed leftMotorSpeed, ControllerTypes::MotorSpeed rightMotorSpeed) const;

    private:
        static ControllerTypes::TriggerValue NormalizeTrigger(unsigned char triggerValue, unsigned char thresholdValue);
        static ControllerTypes::ThumbStickAxis NormalizeStick(short xValue, short yValue, short deadZoneValue);
    };
} // namespace Input