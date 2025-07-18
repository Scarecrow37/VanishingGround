#pragma once
#include "RenderPass.h"

class DownScalePass : public RenderPass
{
public:
    DownScalePass();
    virtual ~DownScalePass();

public:
    void Initialize(RenderScene* ownerScene) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;

private:
    SharedResource<RenderTarget> _renderTarget;
};