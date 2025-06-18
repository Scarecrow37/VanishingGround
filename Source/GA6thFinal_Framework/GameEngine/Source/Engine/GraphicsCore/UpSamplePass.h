#pragma once
#include "RenderPass.h"

class UpSamplePass : public RenderPass
{
public:
    UpSamplePass();
    virtual ~UpSamplePass();

public:
    void Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect) override;
    void Begin(ID3D12GraphicsCommandList* commandList) override;
    void End(ID3D12GraphicsCommandList* commandList) override;
    void Draw(ID3D12GraphicsCommandList* commandList) override;

private:
    ComPtr<ID3D12Resource> _resource;
    DescriptorHandles      _handle;
};