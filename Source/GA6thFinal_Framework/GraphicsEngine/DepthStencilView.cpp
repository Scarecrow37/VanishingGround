#include "pch.h"
#include "DepthStencilView.h"

void DepthStencilView::Initialize(const D3D12_RESOURCE_DESC& desc)
{
    _desc = desc;    
    _resolution = {(UINT)desc.Width, desc.Height};

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
    _desc.Width   = resolution.Width;
    _desc.Height  = resolution.Height;
    _resolution   = resolution;
    _currentState = D3D12_RESOURCE_STATE_PRESENT;

    CreateDepthStencilView();
}

void DepthStencilView::CreateDepthStencilView()
{    
    auto& device = Global::device;

    D3D12_CLEAR_VALUE optClear{.Format = _desc.Format, .DepthStencil = {.Depth = 1.f, .Stencil = 0}};

    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    HRESULT hr = S_OK;
    hr         = device->GetDevice()->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &_desc,
                                                             D3D12_RESOURCE_STATE_PRESENT, &optClear, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderScene::CreateDepthStencil device.GetDevice()->CreateCommittedResource Failed");

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
        .Format = _desc.Format, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE};

    device->GetDevice()->CreateDepthStencilView(_resource.Get(), &dsvDesc, _handle);
    _desc = _resource->GetDesc();
}