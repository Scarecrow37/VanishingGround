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
    void Initialize() override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitializeShader();
    void InitializePSO();

private:    
    SharedResource<UnorderedAccessView> _accumlateBuffer;
    SharedResource<UnorderedAccessView> _revealageBuffer;
};