#pragma once

class StructuredBuffer;
class FrameResource
{
public:
	enum class Type { TRANSFORM, MATERIAL };

public:
	FrameResource();
	~FrameResource();

public:
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return _frameHeap; }

public:
	HRESULT Initialize(const UINT numObjects, const UINT numTextures);
	void CopyDescriptorsSimple(const D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT destStartIndex, UINT numDescriptors);
	void CopyDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles);
	void CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, FrameResource::Type type);

private:
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>		_handles;
	ComPtr<ID3D12DescriptorHeap>					_frameHeap;
	std::unique_ptr<StructuredBuffer>				_structuredBuffer[2];
	D3D12_CPU_DESCRIPTOR_HANDLE						_objectBufferHandle;
	UINT64											_fenceValue;
	UINT											_handleSize;
};