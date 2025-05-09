#include "pch.h"
#include "RenderTarget.h"

HRESULT RenderTarget::Initialize(DXGI_FORMAT format, FLOAT clearColor)
{
    clearValue                  = clearColor;
    _format                     = format;
    ComPtr<ID3D12Device> device = UmDevice.GetDevice();
    D3D12_RESOURCE_DESC  desc{.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                              .Width            = UmDevice.GetMode().Width,
                              .Height           = UmDevice.GetMode().Height,
                              .DepthOrArraySize = 1,
                              .MipLevels        = 1,
                              .Format           = _format,
                              .SampleDesc{.Count   = UmDevice.GetMSAAState() ? (UINT)4 : (UINT)1,
                                          .Quality = UmDevice.GetMSAAState() ? UmDevice.GetMSAAQuality() - 1 : (UINT)0},
                              .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                              .Flags  = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET};

    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);
    
    D3D12_CLEAR_VALUE clearValue{
        .Format = _format,
        .Color  = {clearColor, clearColor, clearColor,1.f},
    };
    // committedReosurce로 임시로 생성
    UmDevice.GetDevice()->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &desc,
                                                        D3D12_RESOURCE_STATE_COMMON, &clearValue,
                                                        IID_PPV_ARGS(_resource.GetAddressOf()));

    HRESULT hr = S_OK;

    hr = UmViewManager.AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, _rtvHandle);
    FAILED_CHECK_BREAK(hr);
    UmDevice.GetDevice()->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);

    return hr;
}

void RenderTarget::CreateShaderResourceView()
{
    // Srv 생성하기(RenderTarget에 대한)
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = _format;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = 1;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle);
    UmDevice.GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
}
