#pragma once

class StructuredBuffer
{
public:
	HRESULT Initialize(const D3D12_CPU_DESCRIPTOR_HANDLE handle, const UINT64 size, const UINT numElements);
	void CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size);

private:
	ComPtr<ID3D12Resource> _uploadBuffer;
	ComPtr<ID3D12Resource> _defaultBuffer;
};