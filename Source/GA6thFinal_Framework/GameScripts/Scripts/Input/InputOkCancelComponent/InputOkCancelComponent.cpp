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
    if (_callback && _onClose)
    {
        PopInputLayer();
        _callback(_result);
        gameObject->SetActive(false);
        _onClose = false;
        _callback = nullptr;
    }
}

void InputOkCancelComponent::GetOkOrCancel(const std::function<void(bool)>& callback) 
{
    if (callback)
    {
        PushInputLayer();
        gameObject->SetActive(true);
        _callback = callback;
    }
}

void InputOkCancelComponent::OnOk(const Input::Controller&) 
{
    if (gameObject->ActiveInHierarchy)
    {
        _result  = true;
        _onClose = true;
    }  
}

void InputOkCancelComponent::OnCancel(const Input::Controller&) 
{
    if (gameObject->ActiveInHierarchy)
    {
        _result  = false;
        _onClose = true;
    }
}