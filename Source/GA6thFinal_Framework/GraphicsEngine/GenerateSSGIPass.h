#pragma once
#include "RenderPass.h"

class SSGITechnique;
class GenerateSSGIPass : public RenderPass
{
public:
    GenerateSSGIPass() = default;
    virtual ~GenerateSSGIPass() = default;

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAnsPSO();

private:
    ComputeFX<GE::CS::GENERATE_SSGI> _fx;
    SSGITechnique*                   _ssgiTech;
};
