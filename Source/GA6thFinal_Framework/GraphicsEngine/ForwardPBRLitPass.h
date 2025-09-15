#pragma once
#include "RenderPass.h"

class ForwardPBRLitPass : public RenderPass
{
public:
    ForwardPBRLitPass() = default;
    virtual ~ForwardPBRLitPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
};
