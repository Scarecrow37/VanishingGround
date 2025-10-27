#include "pch.h"

InputReceiver::~InputReceiver() 
{
    if (nullptr != _isDestroy)
    {
        if (_isPushStack)
            PushInputLayer();

        *_isDestroy = true;
    }
}

bool InputReceiver::PushInputLayer()
{
    auto& inputSystem = ESceneManager::Engine::GetInputSystem();
    return inputSystem.PushReceiverToInputStack(*this);
}

bool InputReceiver::PopInputLayer()
{
    auto& inputSystem = ESceneManager::Engine::GetInputSystem();
    return inputSystem.PopReceiverToInputStack(*this);
}

void InputReceiver::Vibrate(const Input::ControllerTypes::Vibration vibration)
{
    auto& inputSystem = ESceneManager::Engine::GetInputSystem();
    inputSystem.Vibrate(vibration);
}
