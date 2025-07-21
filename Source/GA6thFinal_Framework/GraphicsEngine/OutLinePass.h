#pragma once
#include "RenderPass.h"

class OutLinePass : public RenderPass
{
public:
    OutLinePass();
    virtual ~OutLinePass();

public:
    void Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
};