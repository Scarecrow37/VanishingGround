#pragma once

class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();

public:
    virtual void Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList);
    virtual void Begin(ID3D12GraphicsCommandList* commandList) {};
    virtual void Draw(ID3D12GraphicsCommandList* commandList) {};
    virtual void End(ID3D12GraphicsCommandList* commandList) {};

protected:
    RenderScene* _ownerScene{nullptr};

    std::unique_ptr<ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState>    _pipelineState;

    SharedResource<RenderTarget> _meshRenderTarget;
    SharedResource<RenderTarget> _finalRenderTarget;
};
