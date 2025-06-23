#pragma once
#include "RenderPass.h"
class ParticleResolvePass : public RenderPass
{
public:
    ParticleResolvePass() ;
    virtual ~ParticleResolvePass();

    void Initialize(const D3D12_VIEWPORT& viewport, const D3D12_RECT& scissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void SetAccumulationBuffers(ComPtr<ID3D12Resource> color, ComPtr<ID3D12Resource> alpha,
                                std::vector<DescriptorHandles> handle);

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

    ComPtr<ID3D12Resource>         _accumlateBuffer;
    ComPtr<ID3D12Resource>         _revealageBuffer;
    std::vector<DescriptorHandles> _oitSRVHandles;
};
