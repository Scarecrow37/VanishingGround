#pragma once
#include "RenderPass.h"
class Quad;
class ShaderBuilder;
class ParticleSpritePass : public RenderPass
{
public:
    ParticleSpritePass() = default;
    virtual ~ParticleSpritePass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

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
    std::shared_ptr<Model> _particleQuad;

    // test
    std::shared_ptr<class Texture> _testSprite;
};
