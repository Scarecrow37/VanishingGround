#pragma once
#include "RenderTechnique.h"
class ParticleBloomTechnique : public RenderTechnique
{
public:
    ParticleBloomTechnique();
    virtual ~ParticleBloomTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
};
