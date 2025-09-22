#include "pch.h"
#include "Controller.h"
#include "Adapter.h"

#include <algorithm>

namespace Input
{
    Controller::Controller(const Adapter* adapter)
        : _adapter(adapter), _id(ControllerTypes::INVALID_ID), _state{}, _leftStickBias(StickBias::UNBIASED),
          _rightStickBias(StickBias::UNBIASED)
    {
    }

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
            UpdateStickBias();
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

    Controller::StickBias Controller::GetLeftStickBias() const noexcept
    {
        return _leftStickBias;
    }

    Controller::StickBias Controller::GetRightStickBias() const noexcept
    {
        return _rightStickBias;
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

    Controller::ButtonQueue Controller::GetButtonQueue() const noexcept
    {
        return _queue;
    }

    void Controller::UpdateStickBias()
    {
        _leftStickBias  = StickBias::UNBIASED;
        _rightStickBias = StickBias::UNBIASED;
        std::ranges::for_each(_queue, [this](const ButtonState& buttonState) {
            if (buttonState.Button == Button::LEFT_THUMB_STICK)
            {
                _leftStickBias = buttonState.Bias;
            }
            else if (buttonState.Button == Button::RIGHT_THUMB_STICK)
            {
                _rightStickBias = buttonState.Bias;
            }
        });
    }

} // namespace Input