#pragma once
#include "ResourceBase.h"

class UnorderedAccessView : public ResourceBase
{
public:
    UnorderedAccessView() = default;
    virtual ~UnorderedAccessView() = default;

public:
    D3D12_GPU_DESCRIPTOR_HANDLE        GetUAVHandle() const { return _uavHandle.GPU; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle.GPU; }
    const UINT                         GetID() const { return _ID; }

public:
    void Initialize(const D3D12_RESOURCE_DESC& desc, D3D12_UAV_DIMENSION uavDimension = D3D12_UAV_DIMENSION_TEXTURE2D, D3D12_SRV_DIMENSION srvDimension = D3D12_SRV_DIMENSION_TEXTURE2D);
    void InitializeForBuffer(UINT elementSize, UINT elementCount);
    void ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList);
    void ResourceBarrier(ID3D12GraphicsCommandList* commandList);    
    void ResizeResource(Resolution resolution) override;

private:
    void CreateUnorderedAccessView();

private:
    DescriptorHandles            _srvHandle;
    DescriptorHandles            _uavHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE  _uavCPUHandle;
    ComPtr<ID3D12DescriptorHeap> _cpuDescriptorHeap;
    UINT                         _ID{0};
    D3D12_UAV_DIMENSION          _uavDimension{D3D12_UAV_DIMENSION_UNKNOWN};
    D3D12_SRV_DIMENSION          _srvDimension{D3D12_SRV_DIMENSION_UNKNOWN};
};