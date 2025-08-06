#pragma once
#include "RenderPass.h"
class SSAOWritePass : public RenderPass
{
public:
    SSAOWritePass();
    virtual ~SSAOWritePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                    ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    void                               InitShaderAndPSO();
    FX<GE::VS::QUAD, GE::PS::WRITE_AO> _fxSSAOWrite;
    SharedResource<RenderTarget>       _renderTarget;
};
