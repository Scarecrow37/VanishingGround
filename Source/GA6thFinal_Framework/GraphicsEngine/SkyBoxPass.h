#pragma once
#include "RenderPass.h"

class SkyBox;
class SkyBoxPass : public RenderPass
{
public:
    SkyBoxPass();
    virtual ~SkyBoxPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();

private:
    FX<GE::VS::SKYBOX, GE::PS::SKYBOX> _fx;
    SkyBox* _skyBox;
};
