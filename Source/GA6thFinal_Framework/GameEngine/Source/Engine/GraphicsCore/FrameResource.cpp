#include "pch.h"
#include "FrameResource.h"
#include "StructuredBuffer.h"

FrameResource::FrameResource()
	: _fenceValue(0)
{
    for (auto& buffer : _structuredBuffer)
        buffer = std::make_unique<StructuredBuffer>();
}

FrameResource::~FrameResource()
{	
}

void FrameResource::Initialize(const UINT numObjects, const UINT numTextures)
{
	// Frame Resource 전용 SRV Visible 힙 생성
	HRESULT hr = S_OK;
	ID3D12Device* device = UmDevice.GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC desc
	{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = numTextures + Type::END,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 0
	};

	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_frameHeap));
    FAILED_CHECK_MESSAGE(hr, L"FrameResource::Initialize device->CreateDescriptorHeap Failed");

	_handles.resize(numTextures + Type::END);
	UINT size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = _frameHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < numTextures + Type::END; i++)
	{
		_handles[i] = cpuHandle;
		_handles[i].ptr += i * size;
	}

	_structuredBuffer[Type::TRANSFORM]->Initialize(_handles[Type::TRANSFORM], sizeof(ObjectData), numObjects);
    _structuredBuffer[Type::BONE_MATRIXES]->Initialize(_handles[Type::BONE_MATRIXES], sizeof(BoneMatrixes), numObjects);
	_structuredBuffer[Type::MATERIAL]->Initialize(_handles[Type::MATERIAL], sizeof(MaterialData), numObjects);
}

void FrameResource::CopyDescriptorsSimple(const D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT destStartIndex, UINT numDescriptors)
{
	ID3D12Device* device = UmDevice.GetDevice();

	UINT size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = _frameHeap->GetCPUDescriptorHandleForHeapStart();
	destHandle.ptr += (destStartIndex + Type::END) * size;

	device->CopyDescriptorsSimple(numDescriptors, destHandle, handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FrameResource::CopyDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
{
    ID3D12Device* device = UmDevice.GetDevice();

	UINT count = static_cast<UINT>(handles.size());

	UINT destDescriptorCount = 0;
	UINT srcDescriptorCount = 0;

	device->CopyDescriptors(count, &_handles[Type::END], nullptr,
							count, handles.data(), nullptr, 
							D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FrameResource::CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, FrameResource::Type type)
{		
	_structuredBuffer[type]->CopyStructuredBuffer(commandList, data, size);
}