#pragma once
#include "RenderPass.h"

class ParticleSpritePass : public RenderPass
{
public:
    ParticleSpritePass();
    virtual ~ParticleSpritePass();

public:
    void SetAccumulationBuffers(SharedResource<UnorderedAccessView> color, SharedResource<UnorderedAccessView> alpha);

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitializeShaderAndPSO();

private:    
    SharedResource<UnorderedAccessView> _accumulateBuffer;
    SharedResource<UnorderedAccessView> _revealageBuffer;

    std::vector<UINT>                  _albedoTextureIDs;
    std::unique_ptr<StructuredBuffer> _textureIDBuffer;

    FX<GE::VS::PARTICLE_QUAD, GE::PS::PARTICLE_QUAD> _fx;
};


