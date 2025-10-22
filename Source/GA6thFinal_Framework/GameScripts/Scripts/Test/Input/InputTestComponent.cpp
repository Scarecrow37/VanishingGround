#include "pchScripts.h"
#include "InputTestComponent.h"

UMREAL_COMPONENT(InputTestComponent)

InputTestComponent::InputTestComponent()
{
    _leftTrriger = 0.f;
    _rightTrriger = 0.f;
    _leftStickAxis = {0.f,};
    _rightStickAxis = {0.f,};
}
InputTestComponent::~InputTestComponent() = default;

void InputTestComponent::Awake() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::B, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::X, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::Y, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::LEFT_SHOULDER, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::RIGHT_SHOULDER, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::START, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::BACK, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::DPAD_LEFT, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::DPAD_RIGHT, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::DPAD_UP, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::DPAD_DOWN, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::LEFT_THUMB_BUTTON, Action::PRESSED, this, &InputTestComponent::OnButton);
    BindInputAction(ControllerButton::RIGHT_THUMB_BUTTON, Action::PRESSED, this, &InputTestComponent::OnButton);

    BindInputAction(ControllerButton::LEFT_TRIGGER, Action::HELD, this, &InputTestComponent::OnTrigger);
    BindInputAction(ControllerButton::RIGHT_TRIGGER, Action::HELD, this, &InputTestComponent::OnTrigger);

    BindInputAction(ControllerButton::LEFT_THUMB_STICK, Action::HELD, this, &InputTestComponent::OnThumbStick);
    BindInputAction(ControllerButton::RIGHT_THUMB_STICK, Action::HELD, this, &InputTestComponent::OnThumbStick);
}

void InputTestComponent::Update() 
{
    UpdateInput();
}

void InputTestComponent::UpdateInput() 
{
    float dt = UmTime.DeltaTime();
    const Vector3& right = transform->Right;
    const Vector3& forward = transform->Forward;

    if (_leftTrriger > 0.f)
    {
        transform->Position += Vector3::Down * _leftTrriger * MoveSpeed * dt;
        std::string message = std::format("Left Trriger : {}", _leftTrriger);
        UmLogger.Log(LogLevel::LEVEL_INFO, message);
    }

    if (_rightTrriger > 0.f)
    {
        transform->Position += Vector3::Up * _rightTrriger * MoveSpeed * dt;
        std::string message = std::format("Right Trriger : {}", _rightTrriger);
        UmLogger.Log(LogLevel::LEVEL_INFO, message);
    }

    if (_leftStickAxis.Magnitude > 0.f)
    {
        _velocity = right * _leftStickAxis.X + forward * _leftStickAxis.Y;
        _velocity.Normalize();
        _velocity *= _leftStickAxis.Magnitude * dt * MoveSpeed;
        transform->Position += _velocity;
    }
    else
    {
        std::memset(&_velocity, 0, sizeof(_velocity));
    }

    if (_rightStickAxis.Magnitude > 0.f)
    {
        float angle = _rightStickAxis.X * dt * RotSpeed * _rightStickAxis.Magnitude;
        transform->Rotate(Vector3::Up, angle, Transform::Space::WORLD);
         
        angle = _rightStickAxis.Y * dt * RotSpeed * _rightStickAxis.Magnitude;
        transform->Rotate(Vector3::Right, -angle);
    }

    _leftTrriger = 0.f;
    _rightTrriger = 0.f;
    std::memset(&_leftStickAxis, 0, sizeof(_leftStickAxis));
    std::memset(&_rightStickAxis, 0, sizeof(_rightStickAxis));
}

void InputTestComponent::OnButton(const Input::Controller& controller) 
{
    using namespace Input;
    for (auto& [name, flag] : Controller::BUTTON_FLAG_LIST)
    {
        if (controller.IsButtonDown(flag))
        {
            std::string message = std::format("{} Pressed!", name);
            UmLogger.Log(LogLevel::LEVEL_INFO, message);
        }
    }
    Vibrate(ControllerTypes::VIBRATION_HAMMER_SMASH);
}

void InputTestComponent::OnTrigger(const Input::Controller& controller) 
{
    _leftTrriger  = controller.GetLeftTrigger();
    _rightTrriger = controller.GetRightTrigger();
}

void InputTestComponent::OnThumbStick(const Input::Controller& controller) 
{
    _leftStickAxis = controller.GetLeftThumbStickAxis();
    _rightStickAxis = controller.GetRightThumbStickAxis();
}
