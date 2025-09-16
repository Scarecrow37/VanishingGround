#include "pch.h"
#include "Adapter.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include <Xinput.h>

#pragma comment(lib, "XInput.lib")

namespace Input
{
    namespace
    {
        std::unordered_set<ControllerTypes::ID> ConnectedControllers;
    }

    ControllerTypes::ID Adapter::Connect() const noexcept
    {
        for (ControllerTypes::ID id = 0; id < ControllerTypes::MAX_CONNECTION_COUNT; ++id)
        {
            XINPUT_STATE xState{};
            if (const DWORD result = XInputGetState(id, &xState);
                result == ERROR_SUCCESS && !ConnectedControllers.contains(id))
            {
                ConnectedControllers.insert(id);
                return id;
            }
        }
        return ControllerTypes::INVALID_ID;
    }

    ControllerTypes::State Adapter::ReceiveState(const ControllerTypes::ID id) const
    {

        XINPUT_STATE           xState{};
        ControllerTypes::State state{};

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
        unsigned int buttons = xState.Gamepad.wButtons;
        if (state.LeftThumbStickAxis.Magnitude > 0)
            buttons |= ControllerTypes::LEFT_THUMB_STICK;
        if (state.RightThumbStickAxis.Magnitude > 0)
            buttons |= ControllerTypes::RIGHT_THUMB_STICK;
        if (state.LeftTrigger > 0)
            buttons |= ControllerTypes::LEFT_TRIGGER;
        if (state.RightTrigger > 0)
            buttons |= ControllerTypes::RIGHT_TRIGGER;
        state.Buttons         = static_cast<ControllerTypes::Button>(buttons);
        state.StateGeneration = xState.dwPacketNumber;

        return state;
    }

    namespace
    {
        ControllerTypes::Button VirtualKeyToButton(const WORD virtualKey)
        {
            switch (virtualKey)
            {
            case VK_PAD_A:
                return ControllerTypes::A;
            case VK_PAD_B:
                return ControllerTypes::B;
            case VK_PAD_X:
                return ControllerTypes::X;
            case VK_PAD_Y:
                return ControllerTypes::Y;
            case VK_PAD_LSHOULDER:
                return ControllerTypes::LEFT_SHOULDER;
            case VK_PAD_RSHOULDER:
                return ControllerTypes::RIGHT_SHOULDER;
            case VK_PAD_LTRIGGER:
                return ControllerTypes::LEFT_TRIGGER;
            case VK_PAD_RTRIGGER:
                return ControllerTypes::RIGHT_TRIGGER;
            case VK_PAD_DPAD_UP:
                return ControllerTypes::DPAD_UP;
            case VK_PAD_DPAD_DOWN:
                return ControllerTypes::DPAD_DOWN;
            case VK_PAD_DPAD_LEFT:
                return ControllerTypes::DPAD_LEFT;
            case VK_PAD_DPAD_RIGHT:
                return ControllerTypes::DPAD_RIGHT;
            case VK_PAD_START:
                return ControllerTypes::START;
            case VK_PAD_BACK:
                return ControllerTypes::BACK;
            case VK_PAD_LTHUMB_PRESS:
                return ControllerTypes::LEFT_THUMB_BUTTON;
            case VK_PAD_RTHUMB_PRESS:
                return ControllerTypes::RIGHT_THUMB_BUTTON;
            case VK_PAD_LTHUMB_UP:
            case VK_PAD_LTHUMB_DOWN:
            case VK_PAD_LTHUMB_RIGHT:
            case VK_PAD_LTHUMB_LEFT:
            case VK_PAD_LTHUMB_UPLEFT:
            case VK_PAD_LTHUMB_UPRIGHT:
            case VK_PAD_LTHUMB_DOWNRIGHT:
            case VK_PAD_LTHUMB_DOWNLEFT:
                return ControllerTypes::LEFT_THUMB_STICK;
            case VK_PAD_RTHUMB_UP:
            case VK_PAD_RTHUMB_DOWN:
            case VK_PAD_RTHUMB_RIGHT:
            case VK_PAD_RTHUMB_LEFT:
            case VK_PAD_RTHUMB_UPLEFT:
            case VK_PAD_RTHUMB_UPRIGHT:
            case VK_PAD_RTHUMB_DOWNRIGHT:
            case VK_PAD_RTHUMB_DOWNLEFT:
                return ControllerTypes::RIGHT_THUMB_STICK;
            default:
                return static_cast<ControllerTypes::Button>(0);
            }
        }
    } // namespace

    std::vector<ControllerTypes::Button> Adapter::ReceiveQueue(const ControllerTypes::ID id) const
    {
        std::vector<ControllerTypes::Button> queue;

        DWORD result = ERROR_SUCCESS;

        while (result == ERROR_SUCCESS)
        {
            XINPUT_KEYSTROKE xKeystroke;
            result = XInputGetKeystroke(id, 0, &xKeystroke);

            if (result == ERROR_SUCCESS)
            {
                if (xKeystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN || xKeystroke.Flags & XINPUT_KEYSTROKE_KEYUP)
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

    ControllerTypes::TriggerValue Adapter::NormalizeTrigger(const unsigned char triggerValue, const unsigned char thresholdValue)
    {
        const float trigger   = triggerValue;
        const float threshold = thresholdValue;

        ControllerTypes::TriggerValue result = 0;

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

    ControllerTypes::ThumbStickAxis Adapter::NormalizeStick(const short xValue, const short yValue,
                                                            const short deadZoneValue)
    {
        const float x         = xValue;
        const float y         = yValue;
        const float deadZone  = deadZoneValue;
        const float magnitude = std::sqrt(x * x + y * y);

        ControllerTypes::ThumbStickAxis result{};

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