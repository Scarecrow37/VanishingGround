#pragma once

class RenderScene;
class RenderTarget;
class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();

public:
    void SetClearValue(const Color& clearColor, float depthClear = 1.f, UINT clearStencil = 0);
    void SetOwnerScene(RenderScene* owner);

public:
    virtual void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect);
    virtual void Begin(ID3D12GraphicsCommandList* commandList);
    virtual void Draw(ID3D12GraphicsCommandList* commandList) {};
    virtual void End(ID3D12GraphicsCommandList* commandList);

protected:
    D3D12_VIEWPORT _viewPort;
    D3D12_RECT     _sissorRect;
    Color          _clearColor;
    float          _clearDepth;
    UINT           _clearStencil;

    std::unique_ptr<ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState>    _pipelineState;

    RenderScene*  _ownerScene{nullptr};
    RenderTarget* _meshRenderTarget{nullptr};
    RenderTarget* _finalRenderTarget{nullptr};
};
