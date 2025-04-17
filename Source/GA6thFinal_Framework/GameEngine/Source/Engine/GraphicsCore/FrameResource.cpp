#include "pch.h"
#include "FrameResource.h"
#include "StructuredBuffer.h"

FrameResource::FrameResource()
	: _fenceValue(0)
{
	_structuredBuffer[0] = std::make_unique<StructuredBuffer>();
	_structuredBuffer[1] = std::make_unique<StructuredBuffer>();
}

FrameResource::~FrameResource()
{	
}

HRESULT FrameResource::Initialize(const UINT numObjects, const UINT numTextures)
{
	// Frame Resource 전용 SRV Visible 힙 생성
	HRESULT hr = S_OK;
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = numTextures + 2,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 0
	};

	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_frameHeap.GetAddressOf()));
	FAILED_CHECK_BREAK(hr);

	_handles.resize(numTextures + 2);
	UINT size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = _frameHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < numTextures + 2; i++)
	{
		_handles[i] = cpuHandle;
		_handles[i].ptr += i * size;
	}

	_structuredBuffer[static_cast<int>(Type::TRANSFORM)]->Initialize(_handles[0], sizeof(ObjectData), numObjects);
	_structuredBuffer[static_cast<int>(Type::MATERIAL)]->Initialize(_handles[1], sizeof(MaterialData), numObjects);

	return hr;
}

void FrameResource::CopyDescriptorsSimple(const D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT destStartIndex, UINT numDescriptors)
{
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();

	UINT size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = _frameHeap->GetCPUDescriptorHandleForHeapStart();
	destHandle.ptr += (destStartIndex + 2) * size;

	device->CopyDescriptorsSimple(numDescriptors, destHandle, handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FrameResource::CopyDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
{
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();

	UINT count = static_cast<UINT>(handles.size());

	UINT destDescriptorCount = 0;
	UINT srcDescriptorCount = 0;

	device->CopyDescriptors(count, &_handles[2], nullptr,
							count, handles.data(), nullptr, 
							D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FrameResource::CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, FrameResource::Type type)
{		
	_structuredBuffer[(int)type]->CopyStructuredBuffer(commandList, data, size);
}