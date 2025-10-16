#pragma once
#include "RenderPass.h"

class ParticleResolvePass : public RenderPass
{
public:
    ParticleResolvePass() ;
    virtual ~ParticleResolvePass();

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
    FX<GE::VS::QUAD, GE::PS::PARTICLE_RESOLVE> _fx;
};