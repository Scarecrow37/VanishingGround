#pragma once
#include "RenderPass.h"
class AntiAliasingPass : public RenderPass
{
public:
    AntiAliasingPass() = default;
    virtual ~AntiAliasingPass() = default;

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::QUAD, GE::PS::FXAA> _fx;
};
