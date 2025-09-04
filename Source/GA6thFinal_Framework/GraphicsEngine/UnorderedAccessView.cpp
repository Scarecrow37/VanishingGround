#include "pch.h"
#include "UnorderedAccessView.h"

void UnorderedAccessView::Initialize(const D3D12_RESOURCE_DESC& desc, D3D12_UAV_DIMENSION uavDimension, D3D12_SRV_DIMENSION srvDimension)
{
    UINT maxValue = std::max((UINT)desc.Width, desc.Height);

    UINT mipLevelCount = 1;

    if (1 != desc.MipLevels)
    {
        while (maxValue > 0)
        {
            maxValue /= 2;
            mipLevelCount++;
        }
    }
    _uavCPUHandles.resize(mipLevelCount);
    _uavHandles.resize(mipLevelCount);

    for (UINT i = 0; i < mipLevelCount; i++)
    {
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _uavHandles[i]);
    }

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle, &_ID);

    _desc = desc;
    _srvDimension = srvDimension;
    _uavDimension = uavDimension;
    _currentState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

    _resolution = {(UINT)desc.Width, desc.Height};

    CreateUnorderedAccessView();
}

void UnorderedAccessView::ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList, UINT mipLevel)
{
    float clearColor[4] = {0.f, 0.f, 0.f, 0.f};
    commandList->ClearUnorderedAccessViewFloat(_uavHandles[mipLevel].GPU, _uavCPUHandles[mipLevel], _resource.Get(), clearColor, 0, nullptr);
}

void UnorderedAccessView::ResourceBarrier(ID3D12GraphicsCommandList* commandList)
{
    auto br = CD3DX12_RESOURCE_BARRIER::UAV(_resource.Get());
    commandList->ResourceBarrier(1, &br);
}

void UnorderedAccessView::ResizeResource(Resolution resolution)
{
    _currentState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    _desc.Width = resolution.Width;
    _desc.Height = resolution.Height;

    CreateUnorderedAccessView();
}

void UnorderedAccessView::CreateUnorderedAccessView()
{
    ID3D12Device* device = Global::device->GetDevice();

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT                 hr = S_OK;

    hr                         = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &_desc,
                                                                 D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"UnorderedAccessView::Initialize CreateCommittedResource Failed");

    _desc = _resource->GetDesc();
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format                           = _desc.Format;
    uavDesc.ViewDimension                    = _uavDimension;

    for (UINT16 i = 0; i < _desc.MipLevels; i++)
    {
        if (_uavDimension == D3D12_UAV_DIMENSION_TEXTURE2DARRAY)
        {
            uavDesc.Texture2DArray.MipSlice        = i;
            uavDesc.Texture2DArray.FirstArraySlice = 0;
            uavDesc.Texture2DArray.ArraySize       = _desc.DepthOrArraySize;
            uavDesc.Texture2DArray.PlaneSlice      = 0;
        }
        else if (_uavDimension == D3D12_UAV_DIMENSION_TEXTURE3D)
        {
            uavDesc.Texture3D.MipSlice    = i;
            uavDesc.Texture3D.FirstWSlice = 0;
            uavDesc.Texture3D.WSize       = _desc.DepthOrArraySize >> i;
        }
        device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandles[i].CPU);
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.NumDescriptors             = _desc.MipLevels;
    heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask                   = 0;

    hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_cpuDescriptorHeap));
    FAILED_CHECK_MESSAGE(hr, L"UnorderedAccessView::Initialize CreateDescriptorHeap Failed");

    auto cpuHeap = _cpuDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT16 i = 0; i < _desc.MipLevels; i++)
    {
        _uavCPUHandles[i] = cpuHeap;
        cpuHeap.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavCPUHandles[i]);
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = _desc.Format;
    srvDesc.ViewDimension                   = _srvDimension;    
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture1D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = _desc.MipLevels;

    device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);
}

void UnorderedAccessView::InitializeForBuffer(UINT elementSize, UINT elementCount)
{
    ID3D12Device* device = Global::device->GetDevice();

    const UINT            bufferSize = elementSize * elementCount;
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT                 hr = S_OK;
    hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"UAV Buffer CreateCommittedResource Failed");

    // UAV
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format                           = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.NumElements               = elementCount;
    uavDesc.Buffer.StructureByteStride       = elementSize;

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _uavHandles[0]);
    device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandles[0].CPU);

    // SRV
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format                          = DXGI_FORMAT_UNKNOWN;
    srvDesc.Buffer.StructureByteStride      = elementSize;
    srvDesc.Buffer.NumElements              = elementCount;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle, &_ID);
    device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);

    _currentState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
}