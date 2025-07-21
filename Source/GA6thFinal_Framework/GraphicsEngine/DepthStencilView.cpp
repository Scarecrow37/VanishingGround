#include "pch.h"
#include "DepthStencilView.h"

void DepthStencilView::Initialize(const D3D12_RESOURCE_DESC& desc)
{
    _desc = desc;

    switch (desc.Format)
    {
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        _desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        break;
    case DXGI_FORMAT_D32_FLOAT:
        _desc.Format = DXGI_FORMAT_R32_TYPELESS;
        break;
    }

    _currentState = D3D12_RESOURCE_STATE_PRESENT;

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _handle);

    CreateDepthStencilView();
}

void DepthStencilView::ClearDepthStencilView(ID3D12GraphicsCommandList* commandList)
{
    commandList->ClearDepthStencilView(_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
}

void DepthStencilView::ResizeResource(Resolution resolution)
{
    switch (_desc.Format)
    {
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        _desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        break;
    case DXGI_FORMAT_D32_FLOAT:
        _desc.Format = DXGI_FORMAT_R32_TYPELESS;
        break;
    }

    _desc.Width = resolution.Width;
    _desc.Height = resolution.Height;
    _currentState = D3D12_RESOURCE_STATE_PRESENT;    

    CreateDepthStencilView();
}

void DepthStencilView::CreateDepthStencilView()
{    
    auto& device = Global::device;

    D3D12_RESOURCE_DESC depthDesc{.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                                  .Alignment        = 0,
                                  .Width            = _desc.Width,
                                  .Height           = _desc.Height,
                                  .DepthOrArraySize = 1,
                                  .MipLevels        = 1,
                                  .Format           = _desc.Format,
                                  .SampleDesc{.Count   = device->GetMSAAState() ? (UINT)4 : (UINT)1,
                                              .Quality = device->GetMSAAState() ? (device->GetMSAAQuality() - 1) : 0},
                                  .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                                  .Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL};

    D3D12_CLEAR_VALUE optClear{.DepthStencil{.Depth = 1.f, .Stencil = 0}};
    switch (_desc.Format)
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
    hr         = device->GetDevice()->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &depthDesc,
                                                             D3D12_RESOURCE_STATE_PRESENT, &optClear, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateDepthStencil device.GetDevice()->CreateCommittedResource Failed");

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
        .Format = optClear.Format, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE};

    device->GetDevice()->CreateDepthStencilView(_resource.Get(), &dsvDesc, _handle);

    _desc.Format = optClear.Format;
}