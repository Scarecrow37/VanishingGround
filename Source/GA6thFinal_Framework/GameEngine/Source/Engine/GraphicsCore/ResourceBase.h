#pragma once

class ResourceBase
{
protected:
    ResourceBase();
    virtual ~ResourceBase();

public:
    ID3D12Resource* GetResource() const { return _resource.Get(); }

public:
    void TransitionResource(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after);
    void SetName(std::wstring_view name);

protected:
    ComPtr<ID3D12Resource> _resource;
    D3D12_RESOURCE_STATES  _currentState;
};