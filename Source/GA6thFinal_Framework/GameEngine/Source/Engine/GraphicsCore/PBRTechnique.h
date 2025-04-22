#pragma once
#include "RenderTechnique.h"

class PBRTechnique : public RenderTechnique
{
public:
    PBRTechnique();
    virtual ~PBRTechnique();

public:
    void Initalize() override;
    void Execute(ComPtr<ID3D12GraphicsCommandList> commadList) override;

private:
    void InitalizePBRLightPass();
};
