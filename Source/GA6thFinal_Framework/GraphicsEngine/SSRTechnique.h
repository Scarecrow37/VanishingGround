#pragma once
#include "RenderTechnique.h"
class SSRTechnique : public RenderTechnique
{
public:
    SSRTechnique();
    virtual ~SSRTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
};
