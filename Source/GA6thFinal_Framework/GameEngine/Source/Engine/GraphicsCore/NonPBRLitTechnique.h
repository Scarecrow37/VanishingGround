#pragma once
#include "RenderTechnique.h"

class NonPBRLitTechnique : public RenderTechnique
{
public:
    NonPBRLitTechnique();
    virtual ~NonPBRLitTechnique();

public:
    void Initialize() override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
};
