#pragma once
#include "RenderPass.h"
class VolumetricFogTechnique;
class FogCompositePass : public RenderPass
{
public:
    FogCompositePass() = default;
    virtual ~FogCompositePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();

private:
    VolumetricFogTechnique*                  _volumTech;
    FX<GE::VS::QUAD, GE::PS::VOLUMETRIC_FOG> _fx;
};
