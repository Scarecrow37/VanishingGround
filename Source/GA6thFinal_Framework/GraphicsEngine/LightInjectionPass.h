#pragma once
#include "RenderPass.h"
class VolumetricFogTechnique;
class LightInjectionPass : public RenderPass
{
public:
    LightInjectionPass() = default;
    virtual ~LightInjectionPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void Update(ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;

private:
    void InitShaderAndPSO();

private:
    VolumetricFogTechnique*              _volumTech;
    std::unique_ptr<class ShaderBuilder> _shader;
    ComPtr<ID3D12PipelineState>          _pso;
    std::unique_ptr<Texture>             _noiseTexture;
    std::unique_ptr<ConstantBufferView>  _volumetricFogBuffer;
};
