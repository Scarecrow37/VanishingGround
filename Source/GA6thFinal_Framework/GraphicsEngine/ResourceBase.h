#pragma once

class ResourceBase
{
public:
    ResourceBase();
    virtual ~ResourceBase();

public:
    ID3D12Resource*            GetResource() const { return _resource.Get(); }
    D3D12_GPU_VIRTUAL_ADDRESS  GetGPUVirtualAddress() const { return _resource->GetGPUVirtualAddress(); }
    const Resolution&          GetResolution() const { return _resolution; }
    const D3D12_RESOURCE_DESC& GetResourceDesc() const { return _desc; }
    const D3D12_RESOURCE_DESC& GetDesc() const { return _desc; }

public:
    void         TransitionResource(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after);
    void         SetName(std::wstring_view name);
    virtual void ResizeResource(Resolution resolution) {}

protected:
    D3D12_RESOURCE_DESC    _desc{};
    ComPtr<ID3D12Resource> _resource;
    D3D12_RESOURCE_STATES  _currentState;
    Resolution             _resolution;
};