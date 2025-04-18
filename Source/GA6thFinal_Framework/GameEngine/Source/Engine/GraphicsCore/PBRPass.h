#pragma once
#include "RenderPass.h"
class PBRPass : public RenderPass
{
public:
    PBRPass() = default;
    virtual ~PBRPass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ComPtr<ID3D12GraphicsCommandList> commandList) override;
    void End(ComPtr<ID3D12GraphicsCommandList> commandList) override;
    void Draw(ComPtr<ID3D12GraphicsCommandList> commandList) override;
};
