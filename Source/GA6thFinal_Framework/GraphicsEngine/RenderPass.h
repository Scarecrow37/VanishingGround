#pragma once

class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();

public:
    virtual void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList);
    virtual void AddRenderPassDatas(std::string_view sceneName) {};
    virtual void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) {};
    virtual void Begin(ID3D12GraphicsCommandList* commandList) {};
    virtual void Draw(ID3D12GraphicsCommandList* commandList) {};
    virtual void End(ID3D12GraphicsCommandList* commandList) {};

public:
    void SetEnable(bool enable) { _enable = enable; }
    bool IsEnable() { return _enable; }

private:
    bool _enable = true;

protected:
    SharedResource<RenderTarget>   _meshRenderTarget;
    SharedResource<RenderTarget>   _finalRenderTarget;
    RenderScene*                   _ownerScene{nullptr};
    RenderTechnique*               _ownerTechnique{nullptr};
    RenderTarget*                  _sharedRenderTarget{nullptr};
    ComPtr<ID3D12PipelineState>    _pipelineState;
};
