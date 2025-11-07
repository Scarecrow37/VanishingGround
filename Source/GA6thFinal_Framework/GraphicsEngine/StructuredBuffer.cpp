#include "pch.h"
#include "StructuredBuffer.h"

void StructuredBuffer::Initialize(UINT64 stride, UINT numElements)
{
	HRESULT hr = S_OK;
	ID3D12Device* device = Global::device->GetDevice();

	D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment           = 0;
    resourceDesc.Width               = stride * numElements;
    resourceDesc.Height              = 1;
    resourceDesc.DepthOrArraySize    = 1;
    resourceDesc.MipLevels           = 1;
    resourceDesc.Format              = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count    = 1;
    resourceDesc.SampleDesc.Quality  = 0;
    resourceDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	hr = device->CreateCommittedResource(&defaultHeap,
										 D3D12_HEAP_FLAG_NONE,
										 &resourceDesc,
										 D3D12_RESOURCE_STATE_COMMON,
										 nullptr,
										 IID_PPV_ARGS(&_defaultBuffer));

	FAILED_CHECK_MESSAGE(hr, L"StructuredBuffer::Initialize device->CreateCommittedResource Failed");

	auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	hr = device->CreateCommittedResource(&uploadHeap,
										 D3D12_HEAP_FLAG_NONE,
										 &resourceDesc,
										 D3D12_RESOURCE_STATE_GENERIC_READ,
										 nullptr,
										 IID_PPV_ARGS(&_uploadBuffer));

	FAILED_CHECK_MESSAGE(hr, L"StructuredBuffer::Initialize device->CreateCommittedResource Failed");

	// Copy 가능한 형태로 리소스 상태 전환
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
														D3D12_RESOURCE_STATE_COMMON,
														D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	Global::device->GetCommandList()->ResourceBarrier(1, &barrier);

    _stride = (UINT)stride;
}

void StructuredBuffer::CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, const void* data, UINT count)
{
    Global::device->UpdateBuffer(_uploadBuffer, data, _stride * count);

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
														D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
														D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->ResourceBarrier(1, &barrier);
    commandList->CopyBufferRegion(_defaultBuffer.Get(), 0, _uploadBuffer.Get(), 0, _stride * count);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(_defaultBuffer.Get(),
												   D3D12_RESOURCE_STATE_COPY_DEST,
												   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	commandList->ResourceBarrier(1, &barrier);
}
