#include "pch.h"
#include "RenderTarget.h"

void RenderTarget::Initialize(DXGI_MODE_DESC mode, FLOAT clearColor)
{
    _clearValue = {clearColor, clearColor, clearColor, 1.f};

    _mode = mode;

    _currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    _viewPort = {.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    _scissorRect = {.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};

    UmViewManager.AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, _rtvHandle);
    CreateRenderTargetView();

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle);
    _ID = UmViewManager.GetNumShaderResourceView() - 1;
    CreateShaderResourceView();
}

void RenderTarget::CreateRenderTargetView()
{
    D3D12_RESOURCE_DESC desc{.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                             .Width            = _mode.Width,
                             .Height           = _mode.Height,
                             .DepthOrArraySize = 1,
                             .MipLevels        = 1,
                             .Format           = _mode.Format,
                             .SampleDesc{.Count   = UmDevice.GetMSAAState() ? (UINT)4 : (UINT)1,
                                         .Quality = UmDevice.GetMSAAState() ? UmDevice.GetMSAAQuality() - 1 : (UINT)0},
                             .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                             .Flags  = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET};

    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE clearValue{.Format = _mode.Format,
                                 .Color  = {_clearValue.x, _clearValue.y, _clearValue.z, _clearValue.w}};

    ID3D12Device* device = UmDevice.GetDevice();
    HRESULT       hr     = S_OK;

    hr = device->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_RENDER_TARGET,
                                         &clearValue, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderTarget::Initialize CreateCommittedResource Failed");
    
    device->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);
}

void RenderTarget::CreateShaderResourceView()
{   
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = _mode.Format;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = 1;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    UmDevice.GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);    
}

void RenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* commandList)
{    
    commandList->ClearRenderTargetView(_rtvHandle, _clearValue, 0, nullptr);
}

void RenderTarget::ResizeResource(DXGI_MODE_DESC mode)
{
    _mode = mode;

    _currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    _viewPort    = {.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    _scissorRect = {.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};

    CreateRenderTargetView();
    CreateShaderResourceView();
}