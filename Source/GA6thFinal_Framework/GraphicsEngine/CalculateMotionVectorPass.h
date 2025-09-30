#pragma once
#include "RenderPass.h"

class CalculateMotionVectorPass : public RenderPass
{
public:
    CalculateMotionVectorPass() = default;
    virtual ~CalculateMotionVectorPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechique,
                    ID3D12GraphicsCommandList* commadList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
private:
    void InitShaderAndPSO();

private:
    std::unique_ptr<class ShaderBuilder> _shader;
    class SSGITechnique*                 _ssgiTech;
};
