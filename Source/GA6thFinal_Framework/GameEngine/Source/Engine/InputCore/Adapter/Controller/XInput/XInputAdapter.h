#pragma once
#pragma comment(lib, "XInput.lib")

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
        [[nodiscard]] Controller::ID                 Connect() const noexcept override;
        [[nodiscard]] Controller::State              ReceiveState(Controller::ID id) const override;
        [[nodiscard]] std::queue<Controller::Button> ReceiveQueue(Controller::ID id) const override;

    private:
        static Controller::TriggerValue   NormalizeTrigger(BYTE triggerValue, BYTE thresholdValue);
        static Controller::ThumbStickAxis NormalizeStick(SHORT xValue, SHORT yValue, SHORT deadZoneValue);
    };
} // namespace Input