#pragma once
#include "ResourceBase.h"

class DepthStencilView : public ResourceBase
{
public:
    DepthStencilView()          = default;
    virtual ~DepthStencilView() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVHandle() const { return _handle; }
    const DXGI_FORMAT                  GetFormat() const { return _mode.Format; }

public:
    void Initialize(DXGI_MODE_DESC mode);
    void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList);
    virtual void ResizeResource(DXGI_MODE_DESC mode) override;

private:
    void CreateDepthStencilView();

private:
    D3D12_CPU_DESCRIPTOR_HANDLE _handle{};
};