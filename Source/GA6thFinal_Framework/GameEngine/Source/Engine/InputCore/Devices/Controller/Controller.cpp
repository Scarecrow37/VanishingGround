#include "pch.h"
#include "Controller.h"

namespace Input
{
    Controller::Controller(const ControllerAdapter* adapter) : _adapter(adapter), _id(INVALID_ID), _state{} {}

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
        return _id != INVALID_ID;
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
            _id = INVALID_ID;
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

    std::queue<Controller::Button> Controller::GetButtonQueue() const noexcept
    {
        return _queue;
    }

} // namespace Input