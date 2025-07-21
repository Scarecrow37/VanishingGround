#pragma once

class MultiRenderTargetManager
{
public:
    MultiRenderTargetManager();
    ~MultiRenderTargetManager();

public:
    SharedResource<RenderTarget>                     GetRenderTarget(std::string_view name) const;
    const std::vector<SharedResource<RenderTarget>>& GetRenderTargetGroup(std::string_view groupName);
    SharedResource<RenderTarget>                     GetAvailableRenderTarget();
    const std::list<SharedResource<RenderTarget>>&   GetUsedRenderTargets() const { return _usedRenderTargets; }

public:
    void InitializeRenderTargetPool(UINT size, const D3D12_RESOURCE_DESC& desc);
    void AddRenderTarget(std::string_view name, SharedResource<RenderTarget> renderTarget);
    void AddRenderTargetGroup(std::string_view groupName, std::string_view renderTargetName);
    void AddRenderTargetGroup(std::string_view groupName, std::initializer_list<std::string_view> renderTargetNames);
    void ReturnRenderTarget(const SharedResource<RenderTarget>& renderTarget);

private:
    std::unordered_map<std::string, SharedResource<RenderTarget>>              _renderTargets;
    std::unordered_map<std::string, std::vector<SharedResource<RenderTarget>>> _renderTargetGroup;
    std::list<SharedResource<RenderTarget>>                                    _renderTargetPool;
    std::list<SharedResource<RenderTarget>>                                    _usedRenderTargets;
};