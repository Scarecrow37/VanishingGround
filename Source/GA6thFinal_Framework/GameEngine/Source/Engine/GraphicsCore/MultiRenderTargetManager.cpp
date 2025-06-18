#include "pch.h"
#include "MultiRenderTargetManager.h"
#include "RenderTarget.h"

MultiRenderTargetManager::MultiRenderTargetManager() {}

MultiRenderTargetManager::~MultiRenderTargetManager() {}

RenderTarget* MultiRenderTargetManager::GetRenderTarget(std::string_view name) const
{
    auto iter = _renderTargets.find(name.data());
    GRAPHICS_ASSERT(iter != _renderTargets.end(), L"RenderTarget with the given name does not exist");
    
    return iter->second.get();
}

const std::vector<RenderTarget*>& MultiRenderTargetManager::GetRenderTargetGroup(std::string_view groupName) const
{
    auto iter = _renderTargetGroup.find(groupName.data());
    GRAPHICS_ASSERT(iter != _renderTargetGroup.end(), L"RenderTargetGroup with the given name does not exist");
    
    return iter->second;
}

void MultiRenderTargetManager::AddRenderTarget(std::string_view name, std::unique_ptr<RenderTarget> renderTarget)
{
    auto iter = _renderTargets.find(name.data());
    GRAPHICS_ASSERT(iter != _renderTargets.end(), L"RenderTarget with the same name already register");
    
    _renderTargets.emplace(name.data(), std::move(renderTarget));
}

void MultiRenderTargetManager::AddRenderTargetGroup(std::string_view groupName, std::string_view renderTargetName)
{
    auto iter = _renderTargets.find(renderTargetName.data());
    GRAPHICS_ASSERT(iter != _renderTargets.end(), L"RenderTarget with the given name does not exist");

    _renderTargetGroup[groupName.data()].push_back(iter->second.get());
}

void MultiRenderTargetManager::AddRenderTargetGroup(std::string_view                        groupName,
                                                    std::initializer_list<std::string_view> renderTargetNames)
{
    for (const auto& renderTargetName : renderTargetNames)
    {
        auto iter = _renderTargets.find(renderTargetName.data());
        GRAPHICS_ASSERT(iter != _renderTargets.end(), L"RenderTarget with the given name does not exist");

        _renderTargetGroup[groupName.data()].push_back(iter->second.get());
    }
}