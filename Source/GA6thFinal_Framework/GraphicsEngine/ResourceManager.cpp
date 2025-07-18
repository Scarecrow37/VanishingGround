#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{  
    for (auto& [key, weak] : _resources)
    {
        weak.reset();
    }
}