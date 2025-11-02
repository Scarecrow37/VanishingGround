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
        std::function<void(bool)> callbackFunc = callback;
        UmTime.Invoke(this, 0.1f, [this, callbackFunc] { _callback = callbackFunc; });
    }
}

void InputOkCancelComponent::OnOk(const Input::Controller&) 
{
    if (_callback && EnableInHierarchy)
    {
        _result  = true;
        _onClose = true;
        UmAudio.Play("-901002");
    }  
}

void InputOkCancelComponent::OnCancel(const Input::Controller&) 
{
    if (_callback && EnableInHierarchy)
    {
        _result  = false;
        _onClose = true;
        UmAudio.Play("-901004");
    }
}