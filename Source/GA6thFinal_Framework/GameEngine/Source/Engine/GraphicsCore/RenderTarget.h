#pragma once

class RenderTarget
{
public:
    RenderTarget()  = default;
    ~RenderTarget() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle() const { return _rtvHandle; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle; }
    ComPtr<ID3D12Resource>             GetResource() { return _resource; }

public:
    HRESULT Initialize(DXGI_FORMAT format);
    void    CreateShaderResourceView();

private:
    ComPtr<ID3D12Resource>      _resource;
    D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE _srvHandle;
    DXGI_FORMAT                 _format;
};