#include "pchScripts.h"
#include "InputOkCancelComponent.h"

UMREAL_COMPONENT(InputOkCancelComponent)

InputOkCancelComponent::InputOkCancelComponent() = default;
InputOkCancelComponent::~InputOkCancelComponent() = default;

void InputOkCancelComponent::Awake() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, this, &InputOkCancelComponent::OnOk);
    BindInputAction(ControllerButton::B, Action::PRESSED, this, &InputOkCancelComponent::OnCancel);
}

void InputOkCancelComponent::Update() 
{
    if (_onClose)
    {
        _onClose = false;
        _callback(_result);
        gameObject->SetActive(false);
        PopInputLayer();
    }
}

void InputOkCancelComponent::GetOkOrCancel(const std::function<void(bool)>& callback) 
{
    if (callback)
    {
        gameObject->SetActive(true);
        _callback = callback;
        PushInputLayer();
    }
}

void InputOkCancelComponent::OnOk(const Input::Controller&) 
{
    _result = true;
    _onClose = true;
}

void InputOkCancelComponent::OnCancel(const Input::Controller&) 
{
    _result = false;
    _onClose = true;
}