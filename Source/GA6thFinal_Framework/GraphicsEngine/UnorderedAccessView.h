#pragma once
#include "ResourceBase.h"

class UnorderedAccessView : public ResourceBase
{
public:
    UnorderedAccessView() = default;
    virtual ~UnorderedAccessView() = default;

public:
    D3D12_GPU_DESCRIPTOR_HANDLE        GetUAVHandle(UINT mipLevel = 0) const { return _uavHandles[mipLevel].GPU; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle(UINT mipLevel = 0) const { return _srvHandles[mipLevel].GPU; }
    const UINT                         GetID(UINT mipLevel = 0) const { return _IDs[mipLevel]; }

public:
    void Initialize(const D3D12_RESOURCE_DESC& desc, D3D12_UAV_DIMENSION uavDimension = D3D12_UAV_DIMENSION_TEXTURE2D, D3D12_SRV_DIMENSION srvDimension = D3D12_SRV_DIMENSION_TEXTURE2D);
    void InitializeForBuffer(UINT elementSize, UINT elementCount);
    void ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList, UINT mipLevel = 0);
    void ResourceBarrier(ID3D12GraphicsCommandList* commandList);    
    void ResizeResource(Resolution resolution) override;

private:
    void CreateUnorderedAccessView();

private:
    std::vector<DescriptorHandles>            _srvHandles;
    std::vector<DescriptorHandles>            _uavHandles;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>  _uavCPUHandles;
    ComPtr<ID3D12DescriptorHeap>              _cpuDescriptorHeap;
    std::vector<UINT>                         _IDs{0};
    D3D12_UAV_DIMENSION                       _uavDimension{D3D12_UAV_DIMENSION_UNKNOWN};
    D3D12_SRV_DIMENSION                       _srvDimension{D3D12_SRV_DIMENSION_UNKNOWN};
};