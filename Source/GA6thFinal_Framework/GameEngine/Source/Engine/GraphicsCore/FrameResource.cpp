#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource() {}

FrameResource::~FrameResource() {}

void FrameResource::SetFrameResource(Type type, UINT rootParametorIndex, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootShaderResourceView(rootParametorIndex, _structuredBuffer[type]->GetGPUVirtualAddress());
}

void FrameResource::Initialize(const UINT numObjects)
{
    for (auto& buffer : _structuredBuffer)
        buffer = std::make_unique<StructuredBuffer>();

    _structuredBuffer[Type::TRANSFORM]->Initialize(sizeof(XMMATRIX), numObjects);
    _structuredBuffer[Type::BONE_MATRIXES]->Initialize(sizeof(BoneMatrixes), numObjects);
    _structuredBuffer[Type::MATERIAL]->Initialize(sizeof(MaterialID), numObjects);
}

void FrameResource::CopyStructuredBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, FrameResource::Type type)
{
    _structuredBuffer[type]->CopyStructuredBuffer(commandList, data, size);
}