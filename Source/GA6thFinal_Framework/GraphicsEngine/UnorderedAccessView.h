#pragma once
#include "ResourceBase.h"

class UnorderedAccessView : public ResourceBase
{
public:
    enum class UAVType
    {
        TEXTURE,
        STRUCTURED_BUFFER,
        BYTE_ADDRESS_BUFFER
    };

    enum class UAVSliceType
    {
        PER_MIP,
        PER_ARRAY_SLICE
    };

public:
    UnorderedAccessView() = default;
    virtual ~UnorderedAccessView() = default;

public:
    void InitializeAsTexture(const D3D12_RESOURCE_DESC& desc, UAVSliceType sliceType, bool createSRV = false, D3D12_SRV_DIMENSION srvDimension = D3D12_SRV_DIMENSION_TEXTURE2D);
    void InitializeAsStructuredBuffer(UINT elementCount, UINT stride, bool createSRV = false);
    void InitializeAsByteAddressBuffer(UINT bufferSize, bool createSRV = false);

    void ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList, const Vector4& clearValue, UINT sliceIndex = 0);
    void ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList, const UINT clearValue[4], UINT sliceIndex = 0);
    void ResourceBarrier(ID3D12GraphicsCommandList* commandList);

public:
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetUAVHandle(UINT sliceIndex = 0) const { return _uavHandles[sliceIndex].GPU; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle.GPU; }

private:
    void CreateResource();
    void CreateViews(bool createSRV);

private:
    std::vector<DescriptorHandles> _uavHandles;
    DescriptorHandles              _srvHandle;
    UAVType                        _uavType;
    UAVSliceType                   _sliceType;
    D3D12_SRV_DIMENSION            _srvDimension;

    // For clearing texture UAVs
    ComPtr<ID3D12DescriptorHeap>         _clearCPUHeap;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _clearCPUHandles;

    // For Buffers
    UINT _elementCount;
    UINT _stride;
    UINT _bufferSize;
};