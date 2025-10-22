#pragma once
#include "RenderPass.h"

class SSGITechnique;
class CalculateMotionVectorPass : public RenderPass
{
public:
    CalculateMotionVectorPass() = default;
    virtual ~CalculateMotionVectorPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechique, ID3D12GraphicsCommandList* commadList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitShaderAndPSO();

private:
    ComputeFX<GE::CS::CALCULATE_MOTION_VECTOR> _fx;
    SSGITechnique*                             _ssgiTech;
};
