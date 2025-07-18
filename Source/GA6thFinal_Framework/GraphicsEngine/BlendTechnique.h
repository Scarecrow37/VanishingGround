#pragma once
#include "RenderTechnique.h"

class BlendTechnique : public RenderTechnique
{
public:
    BlendTechnique();
    virtual ~BlendTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
};