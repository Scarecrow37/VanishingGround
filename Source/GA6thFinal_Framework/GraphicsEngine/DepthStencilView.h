#pragma once
#include "ResourceBase.h"

class DepthStencilView : public ResourceBase
{
public:
    DepthStencilView()          = default;
    virtual ~DepthStencilView() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVHandle() const { return _dsvHandle.CPU; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle.CPU; }
    const DXGI_FORMAT                  GetFormat() const { return _desc.Format; }

public:
    void Initialize(const D3D12_RESOURCE_DESC& desc, bool createSRV = false);
    void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList);
    void ResizeResource(SIZE resolution) override;

private:
    void        CreateResrouce();
    void        CreateViews();
    DXGI_FORMAT GetSRVFormat(DXGI_FORMAT dsvFormat);
    void        CreateDepthStencilView();

private:
    bool              _createSRV = false;
    DescriptorHandles _dsvHandle{};
    DescriptorHandles _srvHandle{};
};