#pragma once
#include "ResourceBase.h"

class DepthStencilView : public ResourceBase
{
public:
    DepthStencilView()          = default;
    virtual ~DepthStencilView() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVHandle() const { return _handle; }
    const DXGI_FORMAT                  GetFormat() const { return _format; }

public:
    void Initialize(DXGI_MODE_DESC mode);
    void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList);

private:
    D3D12_CPU_DESCRIPTOR_HANDLE _handle{};
    DXGI_FORMAT                 _format;
};