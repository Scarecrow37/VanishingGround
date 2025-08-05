#pragma once
#include "RenderPass.h"

class BrightExtractPass : public RenderPass
{
public:
    BrightExtractPass();
    virtual ~BrightExtractPass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void AddRenderPassDatas(std::string_view sceneName) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    FX<GE::VS::QUAD, GE::PS::BRIGHT_EXTRACT> _fx;
    ComPtr<ID3D12Resource> _finalTexture;
    DescriptorHandles      _finalHandle;
};