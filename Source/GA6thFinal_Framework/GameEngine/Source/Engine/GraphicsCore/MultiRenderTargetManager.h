#pragma once

class RenderTarget;
class MultiRenderTargetManager
{
public:
    MultiRenderTargetManager();
    ~MultiRenderTargetManager();

public:
    RenderTarget*                                   GetRenderTarget(std::string_view name) const;
    const std::vector<RenderTarget*>&               GetRenderTargetGroup(std::string_view groupName) const;
    RenderTarget*                                   GetAvailableRenderTarget();
    const std::list<std::unique_ptr<RenderTarget>>& GetUsedRenderTargets() const { return _usedRenderTargets; }

public:
    void InitializeRenderTargetPool(UINT size, DXGI_MODE_DESC mode);
    void AddRenderTarget(std::string_view name, std::unique_ptr<RenderTarget> renderTarget);
    void AddRenderTargetGroup(std::string_view groupName, std::string_view renderTargetName);
    void AddRenderTargetGroup(std::string_view groupName, std::initializer_list<std::string_view> renderTargetNames);
    void ReturnRenderTarget(RenderTarget* renderTarget);

private:
    std::unordered_map<std::string, std::unique_ptr<RenderTarget>> _renderTargets;
    std::unordered_map<std::string, std::vector<RenderTarget*>>    _renderTargetGroup;
    std::list<std::unique_ptr<RenderTarget>>                       _renderTargetPool;
    std::list<std::unique_ptr<RenderTarget>>                       _usedRenderTargets;
};