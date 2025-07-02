#pragma once
#include "ResourceBase.h"

class UnorderedAccessView : public ResourceBase
{
public:
    D3D12_GPU_VIRTUAL_ADDRESS          GetGPUVirtualAddress() const { return _resource->GetGPUVirtualAddress(); }
    D3D12_GPU_DESCRIPTOR_HANDLE        GetUAVHandle() const { return _uavHandle.GPU; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return _srvHandle.GPU; }
    const UINT                         GetID() const { return _ID; }

public:
    void Initialize(DXGI_MODE_DESC mode);
    void ClearUnorderedAccessView(ID3D12GraphicsCommandList* commandList);
    void ResourceBarrier(ID3D12GraphicsCommandList* commandList);    
    virtual void ResizeResource(DXGI_MODE_DESC mode) override;

private:
    void CreateUnorderedAccessView();

private:
    DescriptorHandles            _srvHandle;
    DescriptorHandles            _uavHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE  _uavCPUHandle;
    ComPtr<ID3D12DescriptorHeap> _cpuDescriptorHeap;
    UINT                         _ID;
};