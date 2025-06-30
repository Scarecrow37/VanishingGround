#pragma once
#include "RenderPass.h"

class EndlessGridPass : public RenderPass
{
public:
    EndlessGridPass();
    virtual ~EndlessGridPass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
};