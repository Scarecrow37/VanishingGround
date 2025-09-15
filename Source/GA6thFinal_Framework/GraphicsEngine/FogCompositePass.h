#pragma once
#include "RenderPass.h"
class VolumetricFogTechnique;
class FogCompositePass : public RenderPass
{
public:
    FogCompositePass() = default;
    virtual ~FogCompositePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;

private:
    void InitShaderAndPSO();

private:
    VolumetricFogTechnique*                  _volumTech;
    FX<GE::VS::QUAD, GE::PS::VOLUMETRIC_FOG> _fx;
};
