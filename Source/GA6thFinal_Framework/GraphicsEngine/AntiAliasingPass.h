#pragma once
#include "RenderPass.h"
class AntiAliasingPass : public RenderPass
{
public:
    AntiAliasingPass();
    virtual ~AntiAliasingPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::QUAD, GE::PS::FXAA> _fx;
    std::unique_ptr<ConstantBufferView> _constantBuffer;
};
