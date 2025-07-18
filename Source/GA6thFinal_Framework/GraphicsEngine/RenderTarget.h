#pragma once
#include "ResourceBase.h"

class RenderTarget : public ResourceBase
{
public:
    RenderTarget()          = default;
    virtual ~RenderTarget() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle() const { return _rtvHandle; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle.GPU; }    
    const UINT                         GetID() const { return _ID; }
    const float&                       GetClearColor() const { return _clearValue.x; }
    const D3D12_VIEWPORT&              GetViewPort() const { return _viewPort; }
    const D3D12_RECT&                  GetScissorRect() const { return _scissorRect; }

public:
    void Initialize(DXGI_MODE_DESC mode, FLOAT clearColor);
    void ClearRenderTarget(ID3D12GraphicsCommandList* commandList);
    virtual void ResizeResource(DXGI_MODE_DESC mode) override;

private:
    void CreateRenderTargetView();
    void CreateShaderResourceView();

private:
    Color                       _clearValue;
    DescriptorHandles           _srvHandle{};
    D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle{};
    UINT                        _ID;
    D3D12_VIEWPORT              _viewPort;
    D3D12_RECT                  _scissorRect;
};