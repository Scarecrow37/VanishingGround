#pragma once
#include "../../Adapter/Adapter.h"
#include "../../Devices/Controller/Controller.h"

namespace Input
{
    /// <summary>
    /// ControllerAdapter는 컨트롤러에 바인드 되어 연결 및 상태 수신을 위한 인터페이스를 제공합니다.
    /// </summary>
    class ControllerAdapter : public Adapter
    {
    public:
        ControllerAdapter()                                    = default;
        ControllerAdapter(const ControllerAdapter&)            = delete;
        ControllerAdapter& operator=(const ControllerAdapter&) = delete;
        ControllerAdapter(ControllerAdapter&&)                 = delete;
        ControllerAdapter& operator=(ControllerAdapter&&)      = delete;
        virtual ~ControllerAdapter()                           = default;

        [[nodiscard]] virtual Controller::ID    Connect() const noexcept              = 0;
        [[nodiscard]] virtual Controller::State ReceiveState(Controller::ID id) const = 0;
        [[nodiscard]] virtual std::queue<Controller::Button> ReceiveQueue(Controller::ID id) const = 0;
    };
} // namespace Input