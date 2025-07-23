#include "pch.h"
#include "ResourceBase.h"

ResourceBase::ResourceBase() 
    : _resource(nullptr)
    , _currentState(D3D12_RESOURCE_STATE_COMMON)
{
}

ResourceBase::~ResourceBase()
{
}

void ResourceBase::TransitionResource(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES after)
{
    GRAPHICS_ASSERT(_resource.Get(), L"ResourceBase::TransitionResource: resource is null.");
    
    if (_currentState != after)
    {
        auto br = CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), _currentState, after);
        commandList->ResourceBarrier(1, &br);
        _currentState = after;
    }
}

void ResourceBase::SetName(std::wstring_view name)
{
    _resource->SetName(name.data());
}
