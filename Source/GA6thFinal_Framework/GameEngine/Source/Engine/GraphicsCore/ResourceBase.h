#pragma once

class ResourceBase
{
public:
    ResourceBase();
    virtual ~ResourceBase();

public:
    ID3D12Resource*       GetResource() const { return _resource.Get(); }
    const DXGI_MODE_DESC& GetMode() const { return _mode; }

public:
    void TransitionResource(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after);
    void SetName(std::wstring_view name);
    virtual void ResizeResource(DXGI_MODE_DESC mode) {}

protected:
    DXGI_MODE_DESC         _mode;
    ComPtr<ID3D12Resource> _resource;
    D3D12_RESOURCE_STATES  _currentState;
    std::unordered_map<ID3D12GraphicsCommandList*, D3D12_RESOURCE_STATES> _currentStates;
};