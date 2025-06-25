#include "pch.h"
#include "UnorderedAccessView.h"

void UnorderedAccessView::Initialize(DXGI_MODE_DESC mode)
{
    ID3D12Device* device = UmDevice.GetDevice();

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width               = mode.Width;
    desc.Height              = mode.Height;
    desc.DepthOrArraySize    = 1;
    desc.MipLevels           = 1;
    desc.Format              = mode.Format;
    desc.SampleDesc.Count    = 1;
    desc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags               = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT                 hr = S_OK;
    hr                         = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
                                                                 D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"UnorderedAccessView::Initialize CreateCommittedResource Failed");

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format                           = mode.Format;
    uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_TEXTURE2D;

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _uavHandle);
    device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandle.CPU);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.NumDescriptors             = 1;
    heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask                   = 0;

    hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_cpuDescriptorHeap));
    FAILED_CHECK_MESSAGE(hr, L"UnorderedAccessView::Initialize CreateDescriptorHeap Failed");

    _uavCPUHandle = _cpuDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavCPUHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = mode.Format;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels             = 1;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle);
    device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);
    _ID = UmViewManager.GetNumShaderResourceView() - 1;    

    _currentState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
}

void UnorderedAccessView::ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList)
{
    float clearColor[4] = {0.f, 0.f, 0.f, 0.f};
    commandList->ClearUnorderedAccessViewFloat(_uavHandle.GPU, _uavCPUHandle, _resource.Get(), clearColor, 0, nullptr);
}

void UnorderedAccessView::ResourceBarrier(ID3D12GraphicsCommandList* commandList)
{
    auto br = CD3DX12_RESOURCE_BARRIER::UAV(_resource.Get());
    commandList->ResourceBarrier(1, &br);
}