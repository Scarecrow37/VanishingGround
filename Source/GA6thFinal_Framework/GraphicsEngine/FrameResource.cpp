#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource() {}

FrameResource::~FrameResource() {}

void FrameResource::SetFrameResource(UINT index, UINT rootParametorIndex, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootShaderResourceView(rootParametorIndex, _structuredBuffers[index]->GetGPUVirtualAddress());
}

void FrameResource::SetComputeFrameResource(UINT index, UINT rootParametorIndex,
                                               ID3D12GraphicsCommandList* commandList)
{
    commandList->SetComputeRootShaderResourceView(rootParametorIndex,
                                                   _structuredBuffers[index]->GetGPUVirtualAddress());
}

void FrameResource::AddFrameResource(UINT stride, UINT numObject)
{
    std::unique_ptr<StructuredBuffer> buffer = std::make_unique<StructuredBuffer>();
    buffer->Initialize(stride, numObject);

    _structuredBuffers.push_back(std::move(buffer));
}

void FrameResource::CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, UINT index, void* data, UINT size)
{
    _structuredBuffers[index]->CopyStructuredBuffer(commandList, data, size);
}