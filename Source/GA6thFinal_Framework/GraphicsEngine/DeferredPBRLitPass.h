#pragma once
#include "RenderPass.h"

class DeferredPBRLitPass : public RenderPass
{
public:
    DeferredPBRLitPass() = default;
    virtual ~DeferredPBRLitPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;   
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();

private:
    FX<GE::VS::QUAD, GE::PS::PBR_LIGHTING> _fx;
};
