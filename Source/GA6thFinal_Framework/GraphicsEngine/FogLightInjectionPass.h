#pragma once
#include "RenderPass.h"

class VolumetricFogTechnique;
class FogLightInjectionPass : public RenderPass
{
public:
    FogLightInjectionPass() = default;
    virtual ~FogLightInjectionPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;

private:
    void InitShaderAndPSO();

private:
    VolumetricFogTechnique*            _volumTech;
    ComputeFX<GE::CS::LIGHT_INJECTION> _fx;
};
