#pragma once
#include "RenderPass.h"

class ParticleResolvePass : public RenderPass
{
public:
    ParticleResolvePass() ;
    virtual ~ParticleResolvePass();

    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void SetAccumulationBuffers(SharedResource<UnorderedAccessView> color, SharedResource<UnorderedAccessView> alpha);

private:
    void InitializeShader();
    void InitializePSO();
    void InitializeDescriptorHeap();

private:
    std::shared_ptr<ShaderBuilder> _resolveShaderBuilder;
    ComPtr<ID3D12PipelineState>    _resolvePSO;
    ComPtr<ID3D12DescriptorHeap>   _descriptorHeap;
    ID3D12GraphicsCommandList*     _particleRenderCommandList;
    UINT                           _descriptorSize;
    
    SharedResource<UnorderedAccessView> _accumlateBuffer;
    SharedResource<UnorderedAccessView> _revealageBuffer;
};
