#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager() = default;

ResourceManager::~ResourceManager() = default;

void ResourceManager::Update()
{
    std::function<void()> callback = nullptr;

    while (!_callbackQueue.empty())
    {
        if (_callbackQueue.try_pop(callback))
        {
            if (callback)
                callback();
        }
    }
}