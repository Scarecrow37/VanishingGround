#pragma once

#include "../ControllerAdapter.h"

namespace Input
{
    /// <summary>
    /// XInputAdapter는 XInput을 사용하여 컨트롤러의 연결 및 상태 수신을 위한 인터페이스를 제공합니다.
    /// </summary>
    class XInputAdapter final : public ControllerAdapter
    {
    public:
        XInputAdapter() = default;
        [[nodiscard]] Controller::ID Connect() const override;
        Result ReceiveState(Controller::ID id, Controller::State* state) const override;

    private:
        static void NormalizeTrigger(BYTE triggerValue, BYTE thresholdValue,
                                     Controller::TriggerValue* normalizedTrigger);

        static void NormalizeStick(SHORT xValue, SHORT yValue, SHORT deadZoneValue,
                                   Controller::ThumbStickAxis* normalizedStick);
    };
} // namespace Input