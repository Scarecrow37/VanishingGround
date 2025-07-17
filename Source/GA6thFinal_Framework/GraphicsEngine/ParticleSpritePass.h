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
    void Initialize(RenderScene* ownerScene) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitializeShader();
    void InitializePSO();

private:    
    SharedResource<UnorderedAccessView> _accumlateBuffer;
    SharedResource<UnorderedAccessView> _revealageBuffer;

    std::vector<int>                  _albedoTextureIDs;
    std::unique_ptr<StructuredBuffer> _textureIDBuffer;
};


