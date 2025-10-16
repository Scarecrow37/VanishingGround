#pragma once
#include "RenderPass.h"

class VolumetricFogTechnique;
class FogLightAccmulatePass : public RenderPass
{
public:
    FogLightAccmulatePass() = default;
    virtual ~FogLightAccmulatePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();

private:
    ComputeFX<GE::CS::LIGHT_ACCUMULATION> _fx;
    VolumetricFogTechnique*               _volumTech;
};
