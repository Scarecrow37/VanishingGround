#pragma once
#include "RenderPass.h"

class CustomMaterial;
class CustomShaderPass : public RenderPass
{
public:
    CustomShaderPass();
    virtual ~CustomShaderPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    std::vector<std::unique_ptr<CustomMaterial>> _customMaterials;
};