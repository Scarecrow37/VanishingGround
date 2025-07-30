#pragma once
#include "ResourceBase.h"

class RenderTarget : public ResourceBase
{
public:
    RenderTarget()          = default;
    virtual ~RenderTarget() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle(UINT mipLevel = 0) const { return _rtvHandles[mipLevel]; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle.GPU; }
    const UINT                         GetID() const { return _ID; }
    const float&                       GetClearColor() const { return _clearValue.x; }
    const D3D12_VIEWPORT&              GetViewport(UINT mipLevel = 0) const { return _viewPorts[mipLevel]; }
    const D3D12_RECT&                  GetScissorRect(UINT mipLevel = 0) const { return _scissorRects[mipLevel]; }

public:
    void Initialize(const D3D12_RESOURCE_DESC& desc, FLOAT clearColor);
    void ClearRenderTarget(ID3D12GraphicsCommandList* commandList, UINT mipLevel = 0);
    void ResizeResource(Resolution resolution) override;

private:
    void CreateRenderTargetView();
    void CreateShaderResourceView();

private:
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _rtvHandles;
    std::vector<D3D12_VIEWPORT>              _viewPorts;
    std::vector<D3D12_RECT>                  _scissorRects;
    Color                                    _clearValue;
    DescriptorHandles                        _srvHandle{};
    UINT                                     _ID;
};