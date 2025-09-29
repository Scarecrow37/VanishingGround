#pragma once
#include "RenderPass.h"
class SSGICompositePass : public RenderPass
{
public:
    SSGICompositePass() = default;
    virtual ~SSGICompositePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    class SSGITechnique*                     _ssgiTech;
    FX<GE::VS::QUAD, GE::PS::SSGI_COMPOSITE> _fx;
};
