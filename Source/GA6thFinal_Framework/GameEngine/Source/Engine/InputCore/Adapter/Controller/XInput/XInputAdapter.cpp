#include "pch.h"
#include "XInputAdapter.h"

namespace Input
{
    Controller::ID XInputAdapter::Connect() const
    {
        for (Controller::ID id = 0; id < Controller::MAX_CONNECTION_COUNT; ++id)
        {
            XINPUT_STATE xState{};
            if (const DWORD result = XInputGetState(id, &xState);
                result == ERROR_SUCCESS && 
                !_connectedControllers.contains(id))
            {
                _connectedControllers.insert(id);
                return id;
            }
        }
        return Controller::INVALID_ID;
    }

    Result XInputAdapter::ReceiveState(const Controller::ID id, Controller::State* state) const
    {
        XINPUT_STATE xState{};
        switch (const DWORD result = XInputGetState(id, &xState))
        {
        case ERROR_SUCCESS: {
            // 이전 패킷 번호와 비교하여 상태가 변경되었는지 확인
            if (state->StateGeneration == xState.dwPacketNumber)
                return INPUT_ERROR_NO_CHANGE;

            NormalizeStick(xState.Gamepad.sThumbLX, xState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
                           &state->LeftThumbStickAxis);
            NormalizeStick(xState.Gamepad.sThumbRX, xState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                           &state->RightThumbStickAxis);
            NormalizeTrigger(xState.Gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, &state->LeftTrigger);
            NormalizeTrigger(xState.Gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, &state->RightTrigger);
            state->Buttons         = xState.Gamepad.wButtons;
            state->StateGeneration = xState.dwPacketNumber;
            return INPUT_ERROR_SUCCESS;
        }
        case ERROR_DEVICE_NOT_CONNECTED:
            _connectedControllers.erase(id);
            return INPUT_ERROR_LOST_DEVICE;
        default:
            return INPUT_ERROR_UNKNOWN;
        }
    }

    void XInputAdapter::NormalizeTrigger(const BYTE triggerValue, const BYTE thresholdValue,
        Controller::TriggerValue* normalizedTrigger) {
        const float trigger   = triggerValue;
        const float threshold = thresholdValue;

        Controller::TriggerValue result = 0;

        if (trigger > threshold)
        {
            // 최대값을 255로 클리핑
            float adjustedTrigger = std::min<float>(trigger, 255.0f);

            // 데드존을 제외한 값으로 조정
            adjustedTrigger -= threshold;

            // 상대적 범위로 정규화
            result = adjustedTrigger / (255.0f - threshold);
        }

        *normalizedTrigger = result;
    }

    void XInputAdapter::NormalizeStick(const SHORT xValue, const SHORT yValue, const SHORT deadZoneValue,
        Controller::ThumbStickAxis* normalizedStick) {
        const float x         = xValue;
        const float y         = yValue;
        const float deadZone  = deadZoneValue;
        const float magnitude = std::sqrt(x * x + y * y);

        Controller::ThumbStickAxis result{};

        if (magnitude > deadZone)
        {
            // 방향 정규화
            result.X = x / magnitude;
            result.Y = y / magnitude;

            // 최대값을 32767로 클리핑
            float adjustedMagnitude = std::min<float>(magnitude, 32767);

            // 데드존을 제외한 값으로 조정
            adjustedMagnitude -= deadZone;

            // 상대적 범위로 정규화
            result.Magnitude = adjustedMagnitude / (32767 - deadZone);
        }

        *normalizedStick = result;
    }
}