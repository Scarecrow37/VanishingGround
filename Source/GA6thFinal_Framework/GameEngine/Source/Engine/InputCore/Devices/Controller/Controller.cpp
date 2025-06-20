#include "Controller.h"
#include "pch.h"

namespace Input
{
    Controller::Controller(const Adapter* adapter) : _adapter(adapter), _id(INVALID_ID), _state{} {}

    Result Controller::Connect()
    {
        if (IsConnected())
            return INPUT_ERROR_SUCCESS;
        //_id = _adapter->Connect();
        return IsConnected() ? INPUT_ERROR_SUCCESS : INPUT_ERROR_NOT_CONNECTED;
    }

    bool Controller::IsConnected() const
    {
        return _id != INVALID_ID;
    }

    Result Controller::UpdateState()
    {
        if (!IsConnected())
            return INPUT_ERROR_NOT_CONNECTED;
        // return _adapter->ReceiveState(_id, &_state);
    }

    Controller::ThumbStickAxis Controller::GetLeftThumbStickAxis() const
    {
        return _state.LeftThumbStickAxis;
    }

    Controller::ThumbStickAxis Controller::GetRightThumbStickAxis() const
    {
        return _state.RightThumbStickAxis;
    }

    Controller::TriggerValue Controller::GetLeftTrigger() const
    {
        return _state.LeftTrigger;
    }

    Controller::TriggerValue Controller::GetRightTrigger() const
    {
        return _state.RightTrigger;
    }

    bool Controller::IsButtonDown(const Button button) const
    {
        return _state.Buttons & button;
    }

    bool Controller::IsButtonUp(const Button button) const
    {
        return !IsButtonDown(button);
    }

    Controller::ID Controller::GetID() const
    {
        return _id;
    }

} // namespace Input