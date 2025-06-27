#pragma once
#include "RenderTechnique.h"

class BloomTechnique : public RenderTechnique
{
public:
    BloomTechnique();
    virtual ~BloomTechnique();
    bool pauseFlag = false;

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;
};
