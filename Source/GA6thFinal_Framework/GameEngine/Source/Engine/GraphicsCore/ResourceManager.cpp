#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Model.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{  
}

void ResourceManager::Clear()
{
    _resources.clear();
}

