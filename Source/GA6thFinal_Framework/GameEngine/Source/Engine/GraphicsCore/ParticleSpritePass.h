#pragma once
#include "RenderPass.h"
class Quad;
class ShaderBuilder;
class StructuredBuffer;
class UnorderedAccessView;
class ParticleSpritePass : public RenderPass
{
public:
    ParticleSpritePass();
    virtual ~ParticleSpritePass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void SetAccumulationBuffers(UnorderedAccessView* color, UnorderedAccessView* alpha);

private:
    void InitializeShader();
    void InitializePSO();
    void InitializeDescriptorHeap();



private:
    std::shared_ptr<ShaderBuilder>           _spriteParticleShaderBuilder;
    std::vector<ComPtr<ID3D12PipelineState>> _psos;
    ComPtr<ID3D12DescriptorHeap>             _descriptorheap;
    UINT                                     _descriptorSize;


    ID3D12GraphicsCommandList* _particleRenderCommandList;
    std::shared_ptr<Model>     _particleQuad;

    UnorderedAccessView* _accumlateBuffer;
    UnorderedAccessView* _revealageBuffer;

    std::vector<int>                  _albedoTextureIDs;
    std::unique_ptr<StructuredBuffer> _textureIDBuffer;
    ComPtr<ID3D12Resource>            _textureIdConstantBuffer;
};


