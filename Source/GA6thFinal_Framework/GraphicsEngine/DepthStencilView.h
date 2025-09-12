#pragma once
#include "ResourceBase.h"

class DepthStencilView : public ResourceBase
{
public:
    DepthStencilView()          = default;
    virtual ~DepthStencilView() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVHandle() const { return _handle; }
    const DXGI_FORMAT                  GetFormat() const { return _desc.Format; }

public:
    void Initialize(const D3D12_RESOURCE_DESC& desc);
    void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList);
    void ResizeResource(SIZE resolution) override;

private:
    void CreateDepthStencilView();

private:
    D3D12_CPU_DESCRIPTOR_HANDLE _handle{};
};