#include "pch.h"

InputReceiver::InputReceiver() 
{
    Application::AppInputSystem::PushReceiver(this);
}

InputReceiver::~InputReceiver() 
{
    Application::AppInputSystem::EraseReceiver(this);
}
