#pragma once
#include "RenderTechnique.h"

class FontTechnique : public RenderTechnique
{
public:
    FontTechnique();
    virtual ~FontTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
};