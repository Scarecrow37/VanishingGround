#pragma once

class RenderTarget
{
public:
    RenderTarget()  = default;
    ~RenderTarget() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle() const { return _rtvHandle; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle.GPU; }
    ID3D12Resource*                    GetResource() { return _resource.Get(); }
    const UINT                         GetID() const { return _ID; }

public:
    void Initialize(UINT width, UINT height, DXGI_FORMAT format, FLOAT clearColor);
    void CreateShaderResourceView();

public:
    FLOAT clearValue;

private:
    ComPtr<ID3D12Resource>      _resource;
    D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle{};
    DescriptorHandles           _srvHandle{};
    DXGI_FORMAT                 _format{};
    UINT                        _ID;
};