#pragma once
#include "RenderTechnique.h"
class PBRLitTechnique : public RenderTechnique
{
public:
    PBRLitTechnique();
    virtual ~PBRLitTechnique();

public:
    void Initialize() override;
    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitGBufferPass();
    void InitDeferredPass();
};
