#pragma once
#include "RenderPass.h"
class BilateralUpsamplePass : public RenderPass
{
public:
    BilateralUpsamplePass() = default;
    virtual ~BilateralUpsamplePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;

private:
    class SSGITechnique*          _ssgiTech;
    FX<GE::VS::QUAD, GE::PS::BILATERAL_UP_SAMPLE> _fx;
};