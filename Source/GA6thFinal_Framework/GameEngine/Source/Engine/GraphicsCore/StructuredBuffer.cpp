#include "pch.h"
#include "StructuredBuffer.h"

HRESULT StructuredBuffer::Initialize(const D3D12_CPU_DESCRIPTOR_HANDLE handle, const UINT64 size, const UINT numElements)
{
	HRESULT hr = S_OK;
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();	

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = size * numElements;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	hr = device->CreateCommittedResource(&defaultHeap,
										 D3D12_HEAP_FLAG_NONE,
										 &resourceDesc,
										 D3D12_RESOURCE_STATE_COMMON,
										 nullptr,
										 IID_PPV_ARGS(_defaultBuffer.GetAddressOf()));

	FAILED_CHECK_BREAK(hr);

	auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	hr = device->CreateCommittedResource(&uploadHeap,
										 D3D12_HEAP_FLAG_NONE,
										 &resourceDesc,
										 D3D12_RESOURCE_STATE_GENERIC_READ,
										 nullptr,
										 IID_PPV_ARGS(_uploadBuffer.GetAddressOf()));

	FAILED_CHECK_BREAK(hr);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;

	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = (UINT)size;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	device->CreateShaderResourceView(_defaultBuffer.Get(), &srvDesc, handle);

	// Copy 가능한 형태로 리소스 상태 전환
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
														D3D12_RESOURCE_STATE_COMMON,
														D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	UmDevice.GetCommandList()->ResourceBarrier(1, &barrier);

	return hr;
}

void StructuredBuffer::CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size)
{
	void* temp = nullptr;

	_uploadBuffer->Map(0, nullptr, &temp);
	memcpy(temp, data, size);
	_uploadBuffer->Unmap(0, nullptr);

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
														D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
														D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->ResourceBarrier(1, &barrier);
	commandList->CopyBufferRegion(_defaultBuffer.Get(), 0, _uploadBuffer.Get(), 0, size);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
												   D3D12_RESOURCE_STATE_COPY_DEST,
												   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	commandList->ResourceBarrier(1, &barrier);
}
