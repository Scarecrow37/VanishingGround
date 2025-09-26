#include "pch.h"

InputReceiver::~InputReceiver() 
{
    if (nullptr != _isDestroy)
    {
        *_isDestroy = true;
    }
}

void InputReceiver::Vibrate(const Input::ControllerTypes::Vibration vibration)
{
    auto& inputSystem = ESceneManager::Engine::GetInputSystem();
    inputSystem.Vibrate(vibration);
}
