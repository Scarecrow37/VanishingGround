#include "pch.h"
#include "XInputAdapter.h"

namespace Input
{
    namespace
    {
        std::unordered_set<Controller::ID> ConnectedControllers;
    }

    Controller::ID XInputAdapter::Connect() const noexcept
    {
        for (Controller::ID id = 0; id < Controller::MAX_CONNECTION_COUNT; ++id)
        {
            XINPUT_STATE xState{};
            if (const DWORD result = XInputGetState(id, &xState);
                result == ERROR_SUCCESS && !ConnectedControllers.contains(id))
            {
                ConnectedControllers.insert(id);
                return id;
            }
        }
        return Controller::INVALID_ID;
    }

    Controller::State XInputAdapter::ReceiveState(const Controller::ID id) const
    {
        constexpr ThrowIfFailed throwIfFailed;

        XINPUT_STATE      xState{};
        Controller::State state{};

        const DWORD result = XInputGetState(id, &xState);
        if (result == ERROR_DEVICE_NOT_CONNECTED)
        {
            ConnectedControllers.erase(id);
            throw DeviceNotConnectedException("Controller with ID " + std::to_string(id) + " lost connection.");
        }
        if (result != ERROR_SUCCESS)
            throw InputException("Failed to get XInput state for controller ID: " + std::to_string(id));

        state.LeftThumbStickAxis =
            NormalizeStick(xState.Gamepad.sThumbLX, xState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        state.RightThumbStickAxis =
            NormalizeStick(xState.Gamepad.sThumbRX, xState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        state.LeftTrigger      = NormalizeTrigger(xState.Gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
        state.RightTrigger     = NormalizeTrigger(xState.Gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
        unsigned short buttons = static_cast<Controller::Button>(xState.Gamepad.wButtons);
        if (state.LeftThumbStickAxis.Magnitude > 0)
            buttons |= Controller::LEFT_THUMB_STICK;
        if (state.RightThumbStickAxis.Magnitude > 0)
            buttons |= Controller::RIGHT_THUMB_STICK;
        if (state.LeftTrigger > 0)
            buttons |= Controller::LEFT_TRIGGER;
        if (state.RightTrigger > 0)
            buttons |= Controller::RIGHT_TRIGGER;
        state.Buttons         = static_cast<Controller::Button>(buttons);
        state.StateGeneration = xState.dwPacketNumber;

        return state;
    }

    namespace 
    {
        Controller::Button VirtualKeyToButton(const WORD virtualKey)
        {
            switch (virtualKey)
            {
            case VK_PAD_A:
                return Controller::A;
            case VK_PAD_B:
                return Controller::B;
            case VK_PAD_X:
                return Controller::X;
            case VK_PAD_Y:
                return Controller::Y;
            case VK_PAD_RSHOULDER:
                return Controller::LEFT_SHOULDER;
            case VK_PAD_LSHOULDER:
                return Controller::RIGHT_SHOULDER;
            case VK_PAD_LTRIGGER:
                return Controller::LEFT_TRIGGER;
            case VK_PAD_RTRIGGER:
                return Controller::RIGHT_TRIGGER;
            case VK_PAD_DPAD_UP:
                return Controller::DPAD_UP;
            case VK_PAD_DPAD_DOWN:
                return Controller::DPAD_DOWN;
            case VK_PAD_DPAD_LEFT:
                return Controller::DPAD_LEFT;
            case VK_PAD_DPAD_RIGHT:
                return Controller::DPAD_RIGHT;
            case VK_PAD_START:
                return Controller::START;
            case VK_PAD_BACK:
                return Controller::BACK;
            case VK_PAD_LTHUMB_PRESS:
                return Controller::LEFT_THUMB_BUTTON;
            case VK_PAD_RTHUMB_PRESS:
                return Controller::RIGHT_THUMB_BUTTON;
            case VK_PAD_LTHUMB_UP:
            case VK_PAD_LTHUMB_DOWN:
            case VK_PAD_LTHUMB_RIGHT:
            case VK_PAD_LTHUMB_LEFT:
            case VK_PAD_LTHUMB_UPLEFT:
            case VK_PAD_LTHUMB_UPRIGHT:
            case VK_PAD_LTHUMB_DOWNRIGHT:
            case VK_PAD_LTHUMB_DOWNLEFT:
                return Controller::LEFT_THUMB_STICK;
            case VK_PAD_RTHUMB_UP:
            case VK_PAD_RTHUMB_DOWN:
            case VK_PAD_RTHUMB_RIGHT:
            case VK_PAD_RTHUMB_LEFT:
            case VK_PAD_RTHUMB_UPLEFT:
            case VK_PAD_RTHUMB_UPRIGHT:
            case VK_PAD_RTHUMB_DOWNRIGHT:
            case VK_PAD_RTHUMB_DOWNLEFT:
                return Controller::RIGHT_THUMB_STICK;
            default:
                return static_cast<Controller::Button>(0);
            }
        }
    }


    std::vector<Controller::Button> XInputAdapter::ReceiveQueue(const Controller::ID id) const
    {
        constexpr ThrowIfFailed throwIfFailed;

        std::vector<Controller::Button> queue;

        DWORD result = ERROR_SUCCESS;

        while (result == ERROR_SUCCESS)
        {
            XINPUT_KEYSTROKE xKeystroke;
            result = XInputGetKeystroke(id, 0, &xKeystroke);

            if (result == ERROR_SUCCESS)
            {
                if (xKeystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN && xKeystroke.Flags & XINPUT_KEYSTROKE_KEYUP)
                queue.push_back(VirtualKeyToButton(xKeystroke.VirtualKey));
            }
            else if (result == ERROR_DEVICE_NOT_CONNECTED)
            {
                ConnectedControllers.erase(id);
                throw DeviceNotConnectedException("Controller with ID " + std::to_string(id) + " lost connection.");
            }
            else if (result == ERROR_EMPTY)
            {
                break;
            }
            else
            {
                throw InputException("Failed to get XInput keystroke for controller ID: " + std::to_string(id));
            }
        }

        return queue;
    }

    Controller::TriggerValue XInputAdapter::NormalizeTrigger(const BYTE triggerValue, const BYTE thresholdValue)
    {
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

        return result;
    }

    Controller::ThumbStickAxis XInputAdapter::NormalizeStick(const SHORT xValue, const SHORT yValue, const SHORT deadZoneValue)
    {
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

        return result;
    }
} // namespace Input