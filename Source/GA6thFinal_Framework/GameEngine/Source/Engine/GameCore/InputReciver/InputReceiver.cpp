#include "pch.h"

InputReceiver::~InputReceiver() 
{
    for (auto& key : _controllerSet)
    {
        auto& inputSystem = ESceneManager::Engine::GetInputSystem();
        std::erase_if(inputSystem._receivers[(int)key.Button][(int)key.Action], 
        [this](auto& pair) 
        {
            auto& [receiver, func] = pair;
            return this == receiver;
        });
    }
}
