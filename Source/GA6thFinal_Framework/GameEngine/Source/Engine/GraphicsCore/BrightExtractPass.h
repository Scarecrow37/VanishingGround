#pragma once
#include "RenderPass.h"

class BrightExtractPass : public RenderPass
{
public:
    BrightExtractPass();
    virtual ~BrightExtractPass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;
};