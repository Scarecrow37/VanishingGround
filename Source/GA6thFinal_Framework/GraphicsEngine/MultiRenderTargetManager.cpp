#include "pch.h"
#include "MultiRenderTargetManager.h"

MultiRenderTargetManager::MultiRenderTargetManager() {}

MultiRenderTargetManager::~MultiRenderTargetManager() {}

SharedResource<RenderTarget> MultiRenderTargetManager::GetRenderTarget(std::string_view name) const
{
    auto iter = _renderTargets.find(name.data());
    GRAPHICS_ASSERT(iter != _renderTargets.end(), L"RenderTarget with the given name does not exist");
    
    return iter->second;
}

const std::vector<SharedResource<RenderTarget>>& MultiRenderTargetManager::GetRenderTargetGroup(std::string_view groupName)
{
    auto iter = _renderTargetGroup.find(groupName.data());
    GRAPHICS_ASSERT(iter != _renderTargetGroup.end(), L"RenderTargetGroup with the given name does not exist");
    
    return _renderTargetGroup[groupName.data()];
}

SharedResource<RenderTarget> MultiRenderTargetManager::GetAvailableRenderTarget()
{
    if (_renderTargetPool.empty())
    {
        GRAPHICS_ASSERT(false, L"No available RenderTarget in the pool");
        return SharedResource<RenderTarget>();
    }

    _usedRenderTargets.push_back(_renderTargetPool.front());
    _renderTargetPool.pop_front();
    
    return _usedRenderTargets.back();
}

void MultiRenderTargetManager::InitializeRenderTargetPool(UINT size, const D3D12_RESOURCE_DESC& desc)
{
    for (UINT i = 0; i < size; ++i)
    {
        SharedResource<RenderTarget> renderTarget = MakeSharedResource<RenderTarget>();

        renderTarget->Initialize(desc, 0.f);

        _renderTargetPool.push_back(renderTarget);
    }
}

void MultiRenderTargetManager::AddRenderTarget(std::string_view name, SharedResource<RenderTarget> renderTarget)
{
    auto iter = _renderTargets.find(name.data());
    GRAPHICS_ASSERT(iter == _renderTargets.end(), L"RenderTarget with the same name already register");

    _renderTargets.emplace(name.data(), renderTarget);
    Global::dxResourceManager->AddResource(renderTarget);
}

void MultiRenderTargetManager::AddRenderTargetGroup(std::string_view groupName, std::string_view renderTargetName)
{
    auto iter = _renderTargets.find(renderTargetName.data());
    GRAPHICS_ASSERT(iter != _renderTargets.end(), L"RenderTarget with the given name does not exist");

    _renderTargetGroup[groupName.data()].push_back(iter->second);
}

void MultiRenderTargetManager::AddRenderTargetGroup(std::string_view                        groupName,
                                                    std::initializer_list<std::string_view> renderTargetNames)
{
    for (const auto& renderTargetName : renderTargetNames)
    {
        auto iter = _renderTargets.find(renderTargetName.data());
        GRAPHICS_ASSERT(iter != _renderTargets.end(), L"RenderTarget with the given name does not exist");

        _renderTargetGroup[groupName.data()].push_back(iter->second);
    }
}

void MultiRenderTargetManager::ReturnRenderTarget(const SharedResource<RenderTarget>& renderTarget)
{
    GRAPHICS_ASSERT(renderTarget != nullptr, L"RenderTarget pointer is null");
    
    auto iter = std::find_if(_usedRenderTargets.begin(), _usedRenderTargets.end(), [renderTarget](const SharedResource<RenderTarget>& rt) { return rt.Get() == renderTarget.Get(); });
    
    GRAPHICS_ASSERT(iter != _usedRenderTargets.end(), L"RenderTarget not found in used list");
    
    _renderTargetPool.push_back(std::move(*iter));
    _usedRenderTargets.erase(iter);
}