#pragma once
#include "RenderTechnique.h"

class BloomTechnique : public RenderTechnique
{
public:
    BloomTechnique();
    virtual ~BloomTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
};
