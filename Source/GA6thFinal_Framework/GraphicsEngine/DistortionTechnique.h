#pragma once
#include "RenderTechnique.h"
class DistortionTechnique : public RenderTechnique
{
public:
    DistortionTechnique();
    virtual ~DistortionTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
};
