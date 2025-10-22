#pragma once
#include "RenderTechnique.h"

class LightingTechnique : public RenderTechnique
{
public:
    LightingTechnique();
    virtual ~LightingTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
};
