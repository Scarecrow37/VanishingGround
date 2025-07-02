#pragma once

class RenderScene;
class RenderTarget;
class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();

public:
    void SetOwnerScene(RenderScene* owner);

public:
    virtual void Initialize();
    virtual void Begin(ID3D12GraphicsCommandList* commandList);
    virtual void Draw(ID3D12GraphicsCommandList* commandList) {};
    virtual void End(ID3D12GraphicsCommandList* commandList);

protected:
    std::unique_ptr<ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState>    _pipelineState;

    RenderScene*                 _ownerScene;
    SharedResource<RenderTarget> _meshRenderTarget;
    SharedResource<RenderTarget> _finalRenderTarget;
};
