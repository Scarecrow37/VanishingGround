#include "pch.h"
#include "DepthStencilView.h"

void DepthStencilView::Initialize(const D3D12_RESOURCE_DESC& desc, bool createSRV)
{
    _desc         = desc;
    _resolution   = {(LONG)desc.Width, (LONG)desc.Height};
    _createSRV    = createSRV;
    _currentState = D3D12_RESOURCE_STATE_PRESENT;

     Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _dsvHandle);
    CreateResrouce();
    CreateViews();
}

 void DepthStencilView::ClearDepthStencilView(ID3D12GraphicsCommandList* commandList)
{
     commandList->ClearDepthStencilView(_dsvHandle.CPU, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0,
     nullptr);
 }

void DepthStencilView::ResizeResource(SIZE resolution)
{
    _desc.Width   = resolution.cx;
    _desc.Height  = resolution.cy;
    _resolution   = resolution;
    _currentState = D3D12_RESOURCE_STATE_PRESENT;

    CreateResrouce();
    CreateViews();
}

void DepthStencilView::CreateResrouce()
{
    ID3D12Device*           device = Global::device->GetDevice();
    CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE optClear{.Format = _desc.Format, .DepthStencil = {.Depth = 1.f, .Stencil = 0}};

    HRESULT hr = S_OK;
    hr = device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_PRESENT,
                                         &optClear, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateDepthStencil device.GetDevice()->CreateCommittedResource Failed");
    _desc = _resource->GetDesc();
}

void DepthStencilView::CreateViews()
{
    ID3D12Device* device = Global::device->GetDevice();

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format                        = _desc.Format;
    dsvDesc.Flags                         = D3D12_DSV_FLAG_NONE;

    if (_desc.DepthOrArraySize > 1)
    {
        dsvDesc.ViewDimension                  = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
        dsvDesc.Texture2DArray.MipSlice        = 0;
        dsvDesc.Texture2DArray.FirstArraySlice = 0;
        dsvDesc.Texture2DArray.ArraySize       = _desc.DepthOrArraySize;
    }
    else
    {
        dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;
    }
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, _dsvHandle);

    device->CreateDepthStencilView(_resource.Get(), &dsvDesc, _dsvHandle.CPU);

    if (_createSRV)
    {
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format                          = GetSRVFormat(_desc.Format);
        srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (_desc.DepthOrArraySize > 1)
        {
            if (_desc.DepthOrArraySize == 6) 

            {
                srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
                srvDesc.TextureCube.MostDetailedMip     = 0;
                srvDesc.TextureCube.MipLevels           = _desc.MipLevels;
                srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
            }
            else // Texture Array

            {
                srvDesc.ViewDimension                  = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                srvDesc.Texture2DArray.MostDetailedMip = 0;
                srvDesc.Texture2DArray.MipLevels       = _desc.MipLevels;
                srvDesc.Texture2DArray.FirstArraySlice = 0;
                srvDesc.Texture2DArray.ArraySize       = _desc.DepthOrArraySize;
            }
        }
        else
        {
            srvDesc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels       = _desc.MipLevels;
        }
        device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);
    }
}

DXGI_FORMAT DepthStencilView::GetSRVFormat(DXGI_FORMAT dsvFormat)
{
    switch (dsvFormat)
    {
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    case DXGI_FORMAT_D16_UNORM:
        return DXGI_FORMAT_R16_UNORM;
    default:
        return dsvFormat;
    }
}

 void DepthStencilView::CreateDepthStencilView()
{
    auto& device = Global::device;

    D3D12_CLEAR_VALUE optClear{.Format = _desc.Format, .DepthStencil = {.Depth = 1.f, .Stencil = 0}};

    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    HRESULT hr = S_OK;
    hr         = device->GetDevice()->CreateCommittedResource(
        &property, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_PRESENT, &optClear, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateDepthStencil device.GetDevice()->CreateCommittedResource Failed");

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
        .Format = _desc.Format, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE};

    device->GetDevice()->CreateDepthStencilView(_resource.Get(), &dsvDesc, _dsvHandle.CPU);
    _desc = _resource->GetDesc();
}