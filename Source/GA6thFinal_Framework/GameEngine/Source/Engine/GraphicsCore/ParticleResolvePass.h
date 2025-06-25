#pragma once
#include "RenderPass.h"

class UnorderedAccessView;
class ParticleResolvePass : public RenderPass
{
public:
    ParticleResolvePass() ;
    virtual ~ParticleResolvePass();

    void Initialize(const D3D12_VIEWPORT& viewport, const D3D12_RECT& scissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void SetAccumulationBuffers(UnorderedAccessView* color, UnorderedAccessView* alpha);

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


    
    UnorderedAccessView* _accumlateBuffer;
    UnorderedAccessView* _revealageBuffer;

};
