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
        struct StrokeToButton
        {
            using ButtonAndBias = std::pair<ControllerTypes::Button, ControllerTypes::StickBias>;
            ButtonAndBias operator()(const WORD virtualKey) const
            {
                ButtonAndBias result =
                    std::make_pair(ControllerTypes::Button::UNDEFINED, ControllerTypes::StickBias::UNBIASED);
                switch (virtualKey)
                {
                case VK_PAD_A:
                    result.first = ControllerTypes::A;
                    break;
                case VK_PAD_B:
                    result.first = ControllerTypes::B;
                    break;
                case VK_PAD_X:
                    result.first = ControllerTypes::X;
                    break;
                case VK_PAD_Y:
                    result.first = ControllerTypes::Y;
                    break;
                case VK_PAD_LSHOULDER:
                    result.first = ControllerTypes::LEFT_SHOULDER;
                    break;
                case VK_PAD_RSHOULDER:
                    result.first = ControllerTypes::RIGHT_SHOULDER;
                    break;
                case VK_PAD_LTRIGGER:
                    result.first = ControllerTypes::LEFT_TRIGGER;
                    break;
                case VK_PAD_RTRIGGER:
                    result.first = ControllerTypes::RIGHT_TRIGGER;
                    break;
                case VK_PAD_DPAD_UP:
                    result.first = ControllerTypes::DPAD_UP;
                    break;
                case VK_PAD_DPAD_DOWN:
                    result.first = ControllerTypes::DPAD_DOWN;
                    break;
                case VK_PAD_DPAD_LEFT:
                    result.first = ControllerTypes::DPAD_LEFT;
                    break;
                case VK_PAD_DPAD_RIGHT:
                    result.first = ControllerTypes::DPAD_RIGHT;
                    break;
                case VK_PAD_START:
                    result.first = ControllerTypes::START;
                    break;
                case VK_PAD_BACK:
                    result.first = ControllerTypes::BACK;
                    break;
                case VK_PAD_LTHUMB_PRESS:
                    result.first = ControllerTypes::LEFT_THUMB_BUTTON;
                    break;
                case VK_PAD_RTHUMB_PRESS:
                    result.first = ControllerTypes::RIGHT_THUMB_BUTTON;
                    break;
                case VK_PAD_LTHUMB_UP:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_UP;
                    break;
                case VK_PAD_LTHUMB_DOWN:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_DOWN;
                    break;
                case VK_PAD_LTHUMB_RIGHT:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_RIGHT;
                    break;
                case VK_PAD_LTHUMB_LEFT:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_LEFT;
                    break;
                case VK_PAD_LTHUMB_UPLEFT:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_UP_LEFT;
                    break;
                case VK_PAD_LTHUMB_UPRIGHT:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_UP_RIGHT;
                    break;
                case VK_PAD_LTHUMB_DOWNRIGHT:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_DOWN_RIGHT;
                    break;
                case VK_PAD_LTHUMB_DOWNLEFT:
                    result.first  = ControllerTypes::LEFT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_DOWN_LEFT;
                    break;
                case VK_PAD_RTHUMB_UP:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_UP;
                    break;
                case VK_PAD_RTHUMB_DOWN:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_DOWN;
                    break;
                case VK_PAD_RTHUMB_RIGHT:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_RIGHT;
                    break;
                case VK_PAD_RTHUMB_LEFT:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_LEFT;
                    break;
                case VK_PAD_RTHUMB_UPLEFT:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_UP_LEFT;
                    break;
                case VK_PAD_RTHUMB_UPRIGHT:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_UP_RIGHT;
                    break;
                case VK_PAD_RTHUMB_DOWNRIGHT:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_DOWN_RIGHT;
                    break;
                case VK_PAD_RTHUMB_DOWNLEFT:
                    result.first  = ControllerTypes::RIGHT_THUMB_STICK;
                    result.second = ControllerTypes::StickBias::BIAS_DOWN_LEFT;
                    break;
                default:
                    break;
                }
                return result;
            }
        };
        struct FlagToStateFlag
        {
            ControllerTypes::StateFlag operator()(const WORD flag) const
            {
                ControllerTypes::StateFlag result = ControllerTypes::STATE_UNCHANGED;
                switch (flag)
                {
                case XINPUT_KEYSTROKE_KEYDOWN:
                    result = ControllerTypes::STATE_DOWN;
                    break;
                case XINPUT_KEYSTROKE_KEYUP:
                    result = ControllerTypes::STATE_UP;
                    break;
                case XINPUT_KEYSTROKE_REPEAT:
                    result = ControllerTypes::STATE_REPEAT;
                    break;
                default:
                    result = ControllerTypes::STATE_UNCHANGED;
                    break;
                }
                return result;
            }
        };
    } // namespace

    ControllerTypes::ButtonQueue Adapter::ReceiveQueue(const ControllerTypes::ID id) const
    {
        ControllerTypes::ButtonQueue queue;

        DWORD result = ERROR_SUCCESS;

        while (result == ERROR_SUCCESS)
        {
            XINPUT_KEYSTROKE xKeystroke;
            result = XInputGetKeystroke(id, 0, &xKeystroke);

            if (result == ERROR_SUCCESS)
            {
                ControllerTypes::ButtonState state = {};
                auto [button, bias]                = StrokeToButton()(xKeystroke.VirtualKey);
                state.Button                       = button;
                state.Bias                         = bias;
                state.Flag                         = FlagToStateFlag()(xKeystroke.Flags);
                queue.push_back(state);
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

    void Adapter::SetVibration(const ControllerTypes::ID id, const ControllerTypes::MotorSpeed leftMotorSpeed,
        const ControllerTypes::MotorSpeed rightMotorSpeed) const
    {
        XINPUT_VIBRATION vibration{.wLeftMotorSpeed = leftMotorSpeed, .wRightMotorSpeed = rightMotorSpeed};
        const DWORD      result = XInputSetState(id, &vibration);
        if (result == ERROR_DEVICE_NOT_CONNECTED)
        {
            ConnectedControllers.erase(id);
            throw DeviceNotConnectedException("Controller with ID " + std::to_string(id) + " lost connection.");
        }
        if (result != ERROR_SUCCESS)
            throw InputException("Failed to set XInput vibration for controller ID: " + std::to_string(id));
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