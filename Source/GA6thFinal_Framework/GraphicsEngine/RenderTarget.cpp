#include "pch.h"
#include "RenderTarget.h"

void RenderTarget::Initialize(const D3D12_RESOURCE_DESC& desc, FLOAT clearColor)
{
    _clearValue = {clearColor, clearColor, clearColor, 1.f};

    _desc = desc;

    _currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    _viewPort = {.Width = (FLOAT)desc.Width, .Height = (FLOAT)desc.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    _scissorRect = {.right = (LONG)desc.Width, .bottom = (LONG)desc.Height};
    _resolution  = {(UINT)desc.Width, (UINT)desc.Height};

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, _rtvHandle);
    CreateRenderTargetView();

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle);
    _ID = Global::viewManager->GetNumShaderResourceView() - 1;
    CreateShaderResourceView();
}

void RenderTarget::CreateRenderTargetView()
{    
    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE clearValue{.Format = _desc.Format,
                                 .Color  = {_clearValue.x, _clearValue.y, _clearValue.z, _clearValue.w}};

    ID3D12Device* device = Global::device->GetDevice();
    HRESULT       hr     = S_OK;

    hr = device->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_RENDER_TARGET,
                                         &clearValue, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderTarget::Initialize CreateCommittedResource Failed");
    
    device->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);
}

void RenderTarget::CreateShaderResourceView()
{   
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = _desc.Format;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = 1;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    Global::device->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);    
}

void RenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* commandList)
{    
    commandList->ClearRenderTargetView(_rtvHandle, _clearValue, 0, nullptr);
}

void RenderTarget::ResizeResource(Resolution resolution)
{
    _currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    _viewPort    = {.Width = (FLOAT)resolution.Width, .Height = (FLOAT)resolution.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    _scissorRect = {.right = (LONG)resolution.Width, .bottom = (LONG)resolution.Height};
    _resolution  = resolution;

    CreateRenderTargetView();
    CreateShaderResourceView();
}