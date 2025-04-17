#pragma once

class RenderTarget
{
public:
    RenderTarget()  = default;
    ~RenderTarget() = default;

public:
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle() const { return _handle; }
    ComPtr<ID3D12Resource>             GetResource() { return _resource; }

public:
    HRESULT                     Initialize();
    D3D12_CPU_DESCRIPTOR_HANDLE CreateShaderResourceView();

private:
    ComPtr<ID3D12Resource>      _resource;
    D3D12_CPU_DESCRIPTOR_HANDLE _handle;
};