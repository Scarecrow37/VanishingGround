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
    const DXGI_MODE_DESC&              GetMode() const { return _mode; }

public:
    void Initialize(UINT width, UINT height, DXGI_FORMAT format, FLOAT clearColor);
    void CreateShaderResourceView();
    void TransitionResource(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
    void ClearRenderTarget(ID3D12GraphicsCommandList* commandList);

private:
    DXGI_MODE_DESC              _mode;
    Color                       _clearValue;
    DescriptorHandles           _srvHandle{};
    ComPtr<ID3D12Resource>      _resource;
    D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle{};
    UINT                        _ID;
};