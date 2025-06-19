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

RenderTarget* MultiRenderTargetManager::GetAvailableRenderTarget()
{
    if (_renderTargetPool.empty())
    {
        GRAPHICS_ASSERT(false, L"No available RenderTarget in the pool");
        return nullptr;
    }

    auto renderTarget = std::move(_renderTargetPool.front());
    _usedRenderTargets.push_back(std::move(renderTarget));
    _renderTargetPool.pop_front();
    
    return _usedRenderTargets.back().get();
}

void MultiRenderTargetManager::InitializeRenderTargetPool(UINT size, DXGI_MODE_DESC mode)
{
    for (UINT i = 0; i < size; ++i)
    {
        std::unique_ptr<RenderTarget> renderTarget = std::make_unique<RenderTarget>();
        renderTarget->Initialize(mode.Width, mode.Height, mode.Format, 0.f);
        renderTarget->CreateShaderResourceView();

        _renderTargetPool.push_back(std::move(renderTarget));
    }
}

void MultiRenderTargetManager::AddRenderTarget(std::string_view name, std::unique_ptr<RenderTarget> renderTarget)
{
    auto iter = _renderTargets.find(name.data());
    GRAPHICS_ASSERT(iter == _renderTargets.end(), L"RenderTarget with the same name already register");
    
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

void MultiRenderTargetManager::ReturnRenderTarget(RenderTarget* renderTarget)
{
    GRAPHICS_ASSERT(renderTarget != nullptr, L"RenderTarget pointer is null");
    
    auto iter = std::find_if(_usedRenderTargets.begin(), _usedRenderTargets.end(), [renderTarget](const std::unique_ptr<RenderTarget>& rt) { return rt.get() == renderTarget; });
    
    GRAPHICS_ASSERT(iter != _usedRenderTargets.end(), L"RenderTarget not found in used list");
    
    _renderTargetPool.push_back(std::move(*iter));
    _usedRenderTargets.erase(iter);
}