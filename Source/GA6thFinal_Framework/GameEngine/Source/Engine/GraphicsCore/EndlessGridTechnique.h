#pragma once
#include "RenderTechnique.h"

class EndlessGridTechnique : public RenderTechnique
{
public:
    EndlessGridTechnique();
    virtual ~EndlessGridTechnique();

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
};
