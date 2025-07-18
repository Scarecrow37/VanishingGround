#pragma once
#include "RenderPass.h"

class SkyBox;
class SkyBoxPass : public RenderPass
{
public:
    SkyBoxPass();
    virtual ~SkyBoxPass();

public:
    void Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();

private:
    SkyBox* _skyBox;
};
