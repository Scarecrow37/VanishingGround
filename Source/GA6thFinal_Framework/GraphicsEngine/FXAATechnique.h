#pragma once
#include "RenderTechnique.h"
class FXAATechnique : public RenderTechnique
{
public:
    FXAATechnique();
    virtual ~FXAATechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
};
