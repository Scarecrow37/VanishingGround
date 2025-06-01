#pragma once
#include "RenderTechnique.h"
class ParticleRenderTechnique : public RenderTechnique
{

public:
    ParticleRenderTechnique() = default;
    virtual ~ParticleRenderTechnique() = default;

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;

    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:

    void InitializeSpriteParticlePass();


};
