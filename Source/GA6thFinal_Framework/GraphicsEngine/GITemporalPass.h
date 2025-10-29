#pragma once
#include "RenderPass.h"

class SSGITechnique;
class GITemporalPass : public RenderPass
{
public:
    GITemporalPass() = default;
    virtual ~GITemporalPass() = default;

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAnsPSO();

private:
    ComputeFX<GE::CS::SSGI_TEMPORAL> _fx;
    SSGITechnique*                   _ssgiTech;
};
