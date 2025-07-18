#pragma once
#include "RenderPass.h"

class EndlessGridPass : public RenderPass
{
public:
    EndlessGridPass();
    virtual ~EndlessGridPass();

public:
    void Initialize(RenderScene* ownerScene) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
};