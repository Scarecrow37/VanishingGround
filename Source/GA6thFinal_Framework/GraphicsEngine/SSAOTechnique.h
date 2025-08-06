#pragma once
#include "RenderTechnique.h"

class SSAOTechnique : public RenderTechnique
{
public:
    SSAOTechnique();
    virtual ~SSAOTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;  
};
