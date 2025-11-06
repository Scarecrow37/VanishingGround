#include "pch.h"
#include "Controller.h"
#include "Adapter.h"

#include <algorithm>

namespace Input
{
    Controller::Controller(const Adapter* adapter)
        : _adapter(adapter), _id(ControllerTypes::INVALID_ID), _state{}, _leftStickBias(StickBias::UNBIASED),
          _rightStickBias(StickBias::UNBIASED), _nextVibration(ControllerTypes::VIBRATION_EMPTY) {}

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
            ButtonQueue queue = _adapter->ReceiveQueue(_id);
            UpdateStickBias();
            _queue = UpdateQueue(queue);
            UpdateVibration();
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

    void Controller::Vibrate(const ControllerTypes::MotorSpeed leftMotorSpeed,
        const ControllerTypes::MotorSpeed rightMotorSpeed) const
    {
        _adapter->SetVibration(_id, leftMotorSpeed, rightMotorSpeed);
    }

    void Controller::Vibrate(const ControllerTypes::MotorSpeed leftMotorSpeed, const ControllerTypes::MotorSpeed rightMotorSpeed,
        std::chrono::milliseconds duration)
    {
        _adapter->SetVibration(_id, leftMotorSpeed, rightMotorSpeed);
        _vibrationConditionVariable.notify_all();
        if (_vibrationFuture.valid()) _vibrationFuture.wait();
        _vibrationFuture = std::async(std::launch::async,  [this, duration]() {
            std::unique_lock lock(_vibrationMutex);
            if (_vibrationConditionVariable.wait_for(lock, duration) == std::cv_status::timeout)
            {
                _adapter->SetVibration(_id, 0, 0);
            }
        });
    }

    void Controller::Vibrate(const ControllerTypes::Vibration& vibration)
    {
        _nextVibration = vibration;
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

    void Controller::UpdateVibration()
    {
        if (_nextVibration.Duration.count() <= 0) return;
        Vibrate(_nextVibration.LeftMotorSpeed, _nextVibration.RightMotorSpeed, _nextVibration.Duration);
        _nextVibration = ControllerTypes::VIBRATION_EMPTY;
    }

    Controller::ButtonQueue Controller::UpdateQueue(ButtonQueue& queue) const
    {
        ButtonQueue updatedQueue;

        for (unsigned int buttonFlag = 0x000001; buttonFlag <= Button::RIGHT_THUMB_STICK; buttonFlag = buttonFlag << 1)
        {
            Button button = static_cast<Button>(buttonFlag);
            if (std::ranges::any_of(queue, [button](const ButtonState& buttonState) { return buttonState.Button == button; }))
            {
                continue;
            }

            if (IsButtonDown(button))
            {
                ButtonState buttonState{};
                buttonState.Button = button;
                buttonState.Flag   = StateFlag::STATE_REPEAT;

                if (button == Button::LEFT_THUMB_STICK)
                {
                    buttonState.Bias = GetLeftStickBias();
                }
                else if (button == Button::RIGHT_THUMB_STICK)
                {
                    buttonState.Bias = GetRightStickBias();
                }

                updatedQueue.push_back(buttonState);
            }
        }

        std::ranges::move(queue, std::back_inserter(updatedQueue));

        return updatedQueue;
    }

} // namespace Input