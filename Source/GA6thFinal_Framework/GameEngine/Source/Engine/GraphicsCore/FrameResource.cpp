#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource() {}

FrameResource::~FrameResource() {}

void FrameResource::SetFrameResource(UINT index, UINT rootParametorIndex, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootShaderResourceView(rootParametorIndex, _structuredBuffers[index].second->GetGPUVirtualAddress());
}

void FrameResource::AddFrameResource(UINT stride, UINT numObject)
{
    std::pair<UINT, std::unique_ptr<StructuredBuffer>> data;
    data.first = stride;
    data.second = std::make_unique<StructuredBuffer>();
    data.second->Initialize(stride, numObject);

    _structuredBuffers.push_back(std::move(data));
}

void FrameResource::CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, UINT index, void* data)
{
    auto& [size, buffer] = _structuredBuffers[index];
    buffer->CopyStructuredBuffer(commandList, data, size);
}