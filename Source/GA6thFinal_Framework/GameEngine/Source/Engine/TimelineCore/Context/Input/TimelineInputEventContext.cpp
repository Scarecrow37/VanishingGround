#include "pch.h"
#include "TimelineInputEventContext.h"

REGISTER_CLASS(Timeline::EventTrack, Timeline::InputEventContext)

namespace Timeline
{
    REFLECT_FUNCTION(InputEventContext)

    InputEventContext::InputEventContext()  = default;
    InputEventContext::~InputEventContext() = default;

    void InputEventContext::OnNotify() 
    {
        switch ((EventMode)ReflectFields->Event)
        {
        case EVENT_VIBRATION: {
            Vibrate();
            break;
        }
        default:
            break;
        }
    }

    void InputEventContext::SerializedReflectEvent() {}

    void InputEventContext::DeserializedReflectEvent() 
    {
        UpdateVibrateState();
    }

    #define VIBRATION_PRESET(preset)                \
        if (ImGui::Selectable(#preset)) {           \
            UpdateVibrateState(preset);             \
        }                                           \

    using namespace Input::ControllerTypes;

    void InputEventContext::ImGuiDrawPropertysEvent() 
    {
        ImGuiHelper::AlignedText("Event Mode", ImGuiHelper::LEFT, 0.8f);
        if (ImGui::RadioButton("Vibration", ReflectFields->Event == EVENT_VIBRATION))
        {
            ReflectFields->Event = EVENT_VIBRATION;
        }
        ImGui::Separator();

        ImGuiHelper::AlignedText("Option", ImGuiHelper::LEFT, 0.8f);
        switch ((EventMode)ReflectFields->Event)
        {
        case EVENT_VIBRATION: {
            if (ImGui::Button(EditorIcon::ICON_PLAY))
            {
                UpdateVibrateState();
                Vibrate();
            }
            ImGui::SameLine();
            if (ImGui::Button(EditorIcon::ICON_STOP))
            {
                auto& inputSystem = ESceneManager::Engine::GetInputSystem();
                inputSystem.StopVibration();
            }

            float offsetX = ImGui::GetContentRegionAvail().x * 0.6f;

            ImGuiHelper::TextWithVerticalSeparator("Duration (ms)", offsetX);
            if (ImGui::InputInt("##Duration", &ReflectFields->Duration, 100))
            {
                ReflectFields->Duration = std::max(0, ReflectFields->Duration);
            }

            ImGuiHelper::TextWithVerticalSeparator("Left Motor Speed (0~1)", offsetX);
            ImGui::SliderFloat("##LeftMotorSpeed", &ReflectFields->LeftMotorSpeed, 0.0f, 1.0f);

            ImGuiHelper::TextWithVerticalSeparator("Right Motor Speed (0~1)", offsetX);
            ImGui::SliderFloat("##RightMotorSpeed", &ReflectFields->RightMotorSpeed, 0.0f, 1.0f);

            if (ImGui::BeginCombo("##Preset", "Preset"))
            {
                VIBRATION_PRESET(VIBRATION_EMPTY);
                VIBRATION_PRESET(VIBRATION_VEHICLE_REVVING);
                VIBRATION_PRESET(VIBRATION_EXPLOSION);
                VIBRATION_PRESET(VIBRATION_GUN_SHOT);
                VIBRATION_PRESET(VIBRATION_BUTTON_CLICK);
                VIBRATION_PRESET(VIBRATION_SWORD_BASH);
                VIBRATION_PRESET(VIBRATION_DAGGER_STAB);
                VIBRATION_PRESET(VIBRATION_HAMMER_SMASH);
                VIBRATION_PRESET(VIBRATION_TAKE_DAMAGE);
                ImGui::EndCombo();
            }

            UpdateVibrateState();
            break;
        }
        default:
            break;
        }
    }
    
    void InputEventContext::Vibrate() 
    {
        auto& inputSystem = ESceneManager::Engine::GetInputSystem();
        inputSystem.Vibrate(_vibrationState);
    }
    void InputEventContext::UpdateVibrateState() 
    {
        _vibrationState.Duration = std::chrono::milliseconds(ReflectFields->Duration);
        _vibrationState.LeftMotorSpeed  = (unsigned short)(std::clamp(ReflectFields->LeftMotorSpeed, 0.0f, 1.0f) * 65535.0f);
        _vibrationState.RightMotorSpeed = (unsigned short)(std::clamp(ReflectFields->RightMotorSpeed, 0.0f, 1.0f) * 65535.0f);
    }
    void InputEventContext::UpdateVibrateState(const Input::ControllerTypes::Vibration& vibration) 
    {
        ReflectFields->Duration = (int)vibration.Duration.count();
        ReflectFields->LeftMotorSpeed  = std::clamp((float)vibration.LeftMotorSpeed / 65535.0f, 0.0f, 1.0f);
        ReflectFields->RightMotorSpeed = std::clamp((float)vibration.RightMotorSpeed / 65535.0f, 0.0f, 1.0f);
        _vibrationState = vibration;
    }
}