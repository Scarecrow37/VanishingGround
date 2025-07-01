#include "pch.h"
#include "DepthStencilView.h"

void DepthStencilView::Initialize(DXGI_MODE_DESC mode)
{
    auto& device = UmDevice;

    UmViewManager.AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _handle);

    D3D12_RESOURCE_DESC depthDesc{.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                                  .Alignment        = 0,
                                  .Width            = mode.Width,
                                  .Height           = mode.Height,
                                  .DepthOrArraySize = 1,
                                  .MipLevels        = 1,
                                  .Format           = mode.Format,
                                  .SampleDesc{.Count   = device.GetMSAAState() ? (UINT)4 : (UINT)1,
                                              .Quality = device.GetMSAAState() ? (device.GetMSAAQuality() - 1) : 0},
                                  .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                                  .Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL};

    D3D12_CLEAR_VALUE optClear{.DepthStencil{.Depth = 1.f, .Stencil = 0}};
    switch (mode.Format)
    {
    case DXGI_FORMAT_R24G8_TYPELESS:
        optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;
    case DXGI_FORMAT_R32_TYPELESS:
        optClear.Format = DXGI_FORMAT_D32_FLOAT;
        break;
    }

    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    HRESULT hr = S_OK;
    hr         = device.GetDevice()->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &depthDesc,
                                                             D3D12_RESOURCE_STATE_PRESENT, &optClear,
                                                             IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateDepthStencil device.GetDevice()->CreateCommittedResource Failed");

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
        .Format = optClear.Format, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE};

    device.GetDevice()->CreateDepthStencilView(_resource.Get(), &dsvDesc, _handle);

    _currentState = D3D12_RESOURCE_STATE_PRESENT;
    _format       = optClear.Format;
}

void DepthStencilView::ClearDepthStencilView(ID3D12GraphicsCommandList* commandList)
{
    commandList->ClearDepthStencilView(_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
}
