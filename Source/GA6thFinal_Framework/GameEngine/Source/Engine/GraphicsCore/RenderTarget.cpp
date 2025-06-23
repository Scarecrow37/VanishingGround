#include "pch.h"
#include "RenderTarget.h"

void RenderTarget::Initialize(UINT width, UINT height, DXGI_FORMAT format, FLOAT clearColor)
{    

    D3D12_RESOURCE_DESC desc{.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                             .Width            = width,
                             .Height           = height,
                             .DepthOrArraySize = 1,
                             .MipLevels        = 1,
                             .Format           = format,
                             .SampleDesc{.Count   = UmDevice.GetMSAAState() ? (UINT)4 : (UINT)1,
                                         .Quality = UmDevice.GetMSAAState() ? UmDevice.GetMSAAQuality() - 1 : (UINT)0},
                             .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                             .Flags  = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET};

    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE clearValue{
        .Format = format,
        .Color  = {clearColor, clearColor, clearColor, 1.f},
    };

    ID3D12Device* device = UmDevice.GetDevice();
    HRESULT       hr     = S_OK;

    hr = device->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderTarget::Initialize CreateCommittedResource Failed");

    UmViewManager.AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, _rtvHandle);
    device->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);

    _clearValue = {clearColor, clearColor, clearColor, 1.f};

    _mode.Width = width;
    _mode.Height = height;
    _mode.Format = format;    
}

void RenderTarget::CreateShaderResourceView()
{
    // Srv 생성하기(RenderTarget에 대한)
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = _mode.Format;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = 1;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle);
    UmDevice.GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);
    _ID = UmViewManager.GetNumShaderResourceView() - 1;
}

void RenderTarget::TransitionResource(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), before, after);
    commandList->ResourceBarrier(1, &br);
}

void RenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* commandList)
{    
    commandList->ClearRenderTargetView(_rtvHandle, _clearValue, 0, nullptr);
}
