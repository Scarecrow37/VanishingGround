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
    RenderTarget*          _renderTarget{nullptr};
    ComPtr<ID3D12Resource> _brightExtractTexture;
    DescriptorHandles      _brightExtractHandle;
};