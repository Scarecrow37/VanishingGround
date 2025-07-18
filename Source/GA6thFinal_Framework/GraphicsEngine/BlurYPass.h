#pragma once
#include "RenderPass.h"

class BlurYPass : public RenderPass
{
public:
    BlurYPass();
    virtual ~BlurYPass();

public:
    void Initialize(RenderScene* ownerScene) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
};