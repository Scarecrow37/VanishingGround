#pragma once
#include "RenderTechnique.h"

class SkyBoxRenderTechnique:public RenderTechnique
{
public:
    SkyBoxRenderTechnique();
    virtual ~SkyBoxRenderTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList)override;
};