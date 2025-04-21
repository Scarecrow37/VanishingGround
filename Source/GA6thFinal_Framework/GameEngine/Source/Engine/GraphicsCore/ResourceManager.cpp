#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Model.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    int a = 0;
}

void ResourceManager::Update()
{
    _tempResource.clear();

    for (auto& [filePath, type] : _loadQueue)
    {
        switch (type)
        {
        case RESOURCE_TYPE::TEXTURE:
            _tempResource.push_back(UmResourceManager.LoadResource<Texture>(filePath));
            break;
        case RESOURCE_TYPE::MODEL:
            _tempResource.push_back(UmResourceManager.LoadResource<Model>(filePath));
            break;
        case RESOURCE_TYPE::ANIMATION:
            //_tempResource.push_back(UmResourceManager.LoadResource<Animation>(filePath));
            break;
        }
    }

    _loadQueue.clear();
}

void ResourceManager::RegisterLoadQueue(const std::pair<std::filesystem::path, RESOURCE_TYPE>& data)
{
    _loadQueue.emplace_back(data);
}

void ResourceManager::Clear()
{
    _resources.clear();
}

//std::shared_ptr<Resource> ResourceManager::MakeShared(std::string_view type, std::wstring_view path, size_t size)
//{
//    _vectorTest.emplace_back();
//    std::shared_ptr<Resource>& ptr = _vectorTest.back();
//
//    void*                     voidPtr = malloc(size);
//    ptr.reset(reinterpret_cast<Resource*>(voidPtr));
//    _resources[type.data()][path.data()] = ptr;
//
//    return ptr;
//}
