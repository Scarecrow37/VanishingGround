#pragma once
#include "UIPassBase_OIT.h"

class UIResolvePass : public UIPassBase_OIT
{
public:
    UIResolvePass();
    virtual ~UIResolvePass();

public:
    void Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    SharedResource<UnorderedAccessView> _outputBuffer;
    ComputeFX<GE::CS::RESOLVE_UI>       _fx;
};