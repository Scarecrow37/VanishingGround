#include "pch.h"
#include "UnorderedAccessView.h"

void UnorderedAccessView::InitializeAsTexture(const D3D12_RESOURCE_DESC& desc, UAVSliceType sliceType, bool createSRV, D3D12_SRV_DIMENSION srvDimension)
{
    _uavType = UAVType::TEXTURE;
    _sliceType = sliceType;
    _desc = desc;
    _resolution = { (LONG)desc.Width, (LONG)desc.Height };
    _srvDimension = srvDimension;

    CreateResource(); // Create resource first to get actual mip levels

    UINT handleCount = 1;
    if (_sliceType == UAVSliceType::PER_MIP)
    {
        handleCount = _desc.MipLevels > 0 ? _desc.MipLevels : 1; // Use the updated _desc
    }
    else // PER_ARRAY_SLICE
    {
        handleCount = _desc.DepthOrArraySize > 0 ? _desc.DepthOrArraySize : 1;
    }

    _uavHandles.resize(handleCount);
    _clearCPUHandles.resize(handleCount);

    CreateViews(createSRV);
}

void UnorderedAccessView::InitializeAsStructuredBuffer(UINT elementCount, UINT stride, bool createSRV)
{
    _uavType = UAVType::STRUCTURED_BUFFER;
    _elementCount = elementCount;
    _stride = stride;
    _uavHandles.resize(1);
    _clearCPUHandles.resize(1);

    CreateResource();
    CreateViews(createSRV);
}

void UnorderedAccessView::InitializeAsByteAddressBuffer(UINT bufferSize, bool createSRV)
{
    _uavType = UAVType::BYTE_ADDRESS_BUFFER;
    _bufferSize = bufferSize;
    _uavHandles.resize(1);
    _clearCPUHandles.resize(1);

    CreateResource();
    CreateViews(createSRV);
}

void UnorderedAccessView::ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList, const Vector4& clearValue, UINT sliceIndex)
{
    commandList->ClearUnorderedAccessViewFloat(GetUAVHandle(sliceIndex), _clearCPUHandles[sliceIndex], _resource.Get(), (float*)&clearValue, 0, nullptr);
}

void UnorderedAccessView::ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList, const UINT clearValue[4], UINT sliceIndex)
{
    commandList->ClearUnorderedAccessViewUint(GetUAVHandle(sliceIndex), _clearCPUHandles[sliceIndex], _resource.Get(), clearValue, 0, nullptr);
}

void UnorderedAccessView::ResourceBarrier(ID3D12GraphicsCommandList* commandList)
{
    auto br = CD3DX12_RESOURCE_BARRIER::UAV(_resource.Get());
    commandList->ResourceBarrier(1, &br);
}

void UnorderedAccessView::CreateResource()
{
    ID3D12Device* device = Global::device->GetDevice();
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    switch (_uavType)
    {
        case UAVType::TEXTURE:
            // Desc is already set in InitializeAsTexture
            break;
        case UAVType::STRUCTURED_BUFFER:
            _desc = CD3DX12_RESOURCE_DESC::Buffer(_elementCount * _stride, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            break;
        case UAVType::BYTE_ADDRESS_BUFFER:
            _desc = CD3DX12_RESOURCE_DESC::Buffer(_bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            break;
    }

    _currentState = _uavType == UAVType::TEXTURE ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS : D3D12_RESOURCE_STATE_COMMON;

    HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &_desc, _currentState, nullptr, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"UnorderedAccessView CreateCommittedResource Failed");

    _desc = _resource->GetDesc();
}

void UnorderedAccessView::CreateViews(bool createSRV)
{
    ID3D12Device*                    device  = Global::device->GetDevice();
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

    switch (_uavType)
    {
    case UAVType::TEXTURE: {
        UINT handleCount = (UINT)_uavHandles.size();

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.NumDescriptors             = handleCount;
        heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        FAILED_CHECK_MESSAGE(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_clearCPUHeap)),
                             L"UAV Clear Heap Create Failed");

        for (UINT i = 0; i < handleCount; ++i)
        {
            Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _uavHandles[i]);
            _clearCPUHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _clearCPUHeap->GetCPUDescriptorHandleForHeapStart(), i,
                device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

            uavDesc.Format        = _desc.Format;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_UNKNOWN;

            if (_sliceType == UAVSliceType::PER_ARRAY_SLICE)
            {
                uavDesc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                uavDesc.Texture2DArray.MipSlice        = 0;
                uavDesc.Texture2DArray.FirstArraySlice = i;
                uavDesc.Texture2DArray.ArraySize       = 1;
            }
            else // PER_MIP
            {
                if (_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && _desc.DepthOrArraySize > 1)
                {
                    uavDesc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                    uavDesc.Texture2DArray.MipSlice        = i;
                    uavDesc.Texture2DArray.FirstArraySlice = 0;
                    uavDesc.Texture2DArray.ArraySize       = _desc.DepthOrArraySize;
                }
                else if (_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
                {
                    uavDesc.ViewDimension         = D3D12_UAV_DIMENSION_TEXTURE3D;
                    uavDesc.Texture3D.MipSlice    = i;
                    uavDesc.Texture3D.FirstWSlice = 0;
                    uavDesc.Texture3D.WSize       = -1;
                }
                else // Standard 1D/2D Texture
                {
                    uavDesc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
                    uavDesc.Texture2D.MipSlice = i;
                }
            }

            device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandles[i].CPU);
            device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _clearCPUHandles[i]);
        }
        break;
    }
    case UAVType::STRUCTURED_BUFFER:
    case UAVType::BYTE_ADDRESS_BUFFER: {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.NumDescriptors             = 1;
        heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        FAILED_CHECK_MESSAGE(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_clearCPUHeap)),
                             L"UAV Clear Heap Create Failed");
        _clearCPUHandles[0] = _clearCPUHeap->GetCPUDescriptorHandleForHeapStart();

        Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _uavHandles[0]);

        if (_uavType == UAVType::STRUCTURED_BUFFER)
        {
            uavDesc.Format                     = DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension              = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.NumElements         = _elementCount;
            uavDesc.Buffer.StructureByteStride = _stride;
        }
        else // BYTE_ADDRESS_BUFFER
        {
            uavDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
            uavDesc.ViewDimension      = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.NumElements = _bufferSize / 4;
            uavDesc.Buffer.Flags       = D3D12_BUFFER_UAV_FLAG_RAW;
        }

        device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandles[0].CPU);
        device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _clearCPUHandles[0]);
        break;
    }
    }

    if (createSRV)
    {
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        switch (_uavType)
        {
        case UAVType::TEXTURE:
            srvDesc.Format        = _desc.Format;
            srvDesc.ViewDimension = _srvDimension;
            if (_srvDimension == D3D12_SRV_DIMENSION_TEXTURE2D)
            {
                srvDesc.Texture2D.MipLevels = _desc.MipLevels;
            }
            else if (_srvDimension == D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
            {
                srvDesc.Texture2DArray.MipLevels = _desc.MipLevels;
                srvDesc.Texture2DArray.ArraySize = _desc.DepthOrArraySize;
            }
            else if (_srvDimension == D3D12_SRV_DIMENSION_TEXTURE3D)
            {
                srvDesc.Texture3D.MipLevels = _desc.MipLevels;
            }
            else if (_srvDimension == D3D12_SRV_DIMENSION_TEXTURECUBE)
            {
                srvDesc.TextureCube.MipLevels = _desc.MipLevels;
            }
            break;
        case UAVType::STRUCTURED_BUFFER:
            srvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
            srvDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.NumElements         = _elementCount;
            srvDesc.Buffer.StructureByteStride = _stride;
            break;
        case UAVType::BYTE_ADDRESS_BUFFER:
            srvDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.ViewDimension      = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.NumElements = _bufferSize / 4;
            srvDesc.Buffer.Flags       = D3D12_BUFFER_SRV_FLAG_RAW;
            break;
        }

        device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);
    }
}
