#pragma once
#include "RenderPass.h"

class ParticleSpritePass : public RenderPass
{
public:
    ParticleSpritePass();
    virtual ~ParticleSpritePass();

public:
    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void SetAccumulationBuffers(SharedResource<UnorderedAccessView> color, SharedResource<UnorderedAccessView> alpha);

private:
    void InitializeShader();
    void InitializePSO();

private:
    std::shared_ptr<ShaderBuilder>           _spriteParticleShaderBuilder;
    std::vector<ComPtr<ID3D12PipelineState>> _psos;

    ID3D12GraphicsCommandList* _particleRenderCommandList;
    std::shared_ptr<Model>     _particleQuad;

    SharedResource<UnorderedAccessView> _accumlateBuffer;
    SharedResource<UnorderedAccessView> _revealageBuffer;

    std::vector<int>                  _albedoTextureIDs;
    std::unique_ptr<StructuredBuffer> _textureIDBuffer;
    ComPtr<ID3D12Resource>            _textureIdConstantBuffer;
};


