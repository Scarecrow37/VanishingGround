#pragma once
#include "RenderPass.h"

class OutLinePass : public RenderPass
{
public:
    OutLinePass();
    virtual ~OutLinePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::QUAD, GE::PS::OUTLINE> _fx;
};