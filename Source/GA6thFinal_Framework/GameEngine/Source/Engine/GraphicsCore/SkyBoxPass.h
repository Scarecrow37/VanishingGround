#pragma once
#include "RenderPass.h"
class SkyBox;
class SkyBoxPass : public RenderPass
{
public:
    SkyBoxPass();
    virtual ~SkyBoxPass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();

private:
    std::unique_ptr<SkyBox> _skyBox;
};
