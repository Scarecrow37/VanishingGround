#pragma once

class StructuredBuffer
{
public:
    const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return _uploadBuffer->GetGPUVirtualAddress(); };

public
    :
    void Initialize(const UINT64 size, const UINT numElements);
	void CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size);

private:
	ComPtr<ID3D12Resource> _uploadBuffer;
	ComPtr<ID3D12Resource> _defaultBuffer;
};