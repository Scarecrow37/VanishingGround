#pragma once

class StructuredBuffer
{
public:
    const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return _uploadBuffer->GetGPUVirtualAddress(); };

public:
    void Initialize(UINT64 stride, UINT numElements);
    void CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, const void* data, UINT count);

private:
	ComPtr<ID3D12Resource> _uploadBuffer;
	ComPtr<ID3D12Resource> _defaultBuffer;
    UINT                   _stride;
};