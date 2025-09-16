#include "pch.h"
#include "Controller.h"
#include "Adapter.h"

namespace Input
{
    Controller::Controller(const Adapter* adapter) : _adapter(adapter), _id(ControllerTypes::INVALID_ID), _state{} {}

    void Controller::Connect()
    {
        if (IsConnected())
            return;
        _id = _adapter->Connect();
        if (!IsConnected())
            throw DeviceNotConnectedException("Failed to connect to controller.");
    }

    bool Controller::IsConnected() const noexcept
    {
        return _id != ControllerTypes::INVALID_ID;
    }

    void Controller::UpdateState()
    {
        if (!IsConnected())
            throw DeviceNotConnectedException("Controller is not connected.");
        try
        {
            _state = _adapter->ReceiveState(_id);
            _queue = _adapter->ReceiveQueue(_id);
        }
        catch (const DeviceNotConnectedException&)
        {
            _id = ControllerTypes::INVALID_ID;
            throw;
        }
    }

    Controller::ThumbStickAxis Controller::GetLeftThumbStickAxis() const noexcept
    {
        return _state.LeftThumbStickAxis;
    }

    Controller::ThumbStickAxis Controller::GetRightThumbStickAxis() const noexcept
    {
        return _state.RightThumbStickAxis;
    }

    struct AxisToBias
    {
        Controller::StickBias operator()(const Controller::ThumbStickAxis& axis, const float threshold) const noexcept
        {
            Controller::StickBias bias;
            if (axis.Magnitude < threshold)
                bias = Controller::StickBias::UNBIASED;
            else if (std::abs(axis.X) > std::abs(axis.Y))
                bias = axis.X > 0 ? Controller::StickBias::RIGHT : Controller::StickBias::LEFT;
            else
                bias = axis.Y > 0 ? Controller::StickBias::UP : Controller::StickBias::DOWN;

            return bias;
        }
    };

    Controller::StickBias Controller::GetLeftStickBias(const float threshold) const noexcept
    {
        return AxisToBias()(GetLeftThumbStickAxis(), threshold);
    }

    Controller::StickBias Controller::GetRightStickBias(const float threshold) const noexcept
    {
        return AxisToBias()(GetRightThumbStickAxis(), threshold);
    }

    Controller::TriggerValue Controller::GetLeftTrigger() const noexcept
    {
        return _state.LeftTrigger;
    }

    Controller::TriggerValue Controller::GetRightTrigger() const noexcept
    {
        return _state.RightTrigger;
    }

    bool Controller::IsButtonDown(const Button button) const noexcept
    {
        return _state.Buttons & button;
    }

    bool Controller::IsButtonUp(const Button button) const noexcept
    {
        return !IsButtonDown(button);
    }

    Controller::ID Controller::GetID() const noexcept
    {
        return _id;
    }

    std::vector<Controller::Button> Controller::GetButtonQueue() const noexcept
    {
        return _queue;
    }

} // namespace Input