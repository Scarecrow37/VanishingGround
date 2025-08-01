#include "pch.h"
#include "DXRSkeletalMesh.h"
#include "VIBuffer.h"
DXRSkeletalMesh::DXRSkeletalMesh(VIBuffer* vibuffer) 
{
    _vibuffer = vibuffer;
}

void DXRSkeletalMesh::Initialize(UINT vertexCount, UINT vertexStride) 
{
    
    Global::device->CreateDefaultBuffer(vertexCount * vertexStride, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, _skinnedVertexBuffer);
    _ID = Global::viewManager->GetNumVertexBuffer();
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::VERTEX_BUFFER_SHADER_RESOURCE,_skinnedVertexBufferSRV);
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = DXGI_FORMAT_UNKNOWN; // Structured buffer일 경우
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.FirstElement             = 0;
    srvDesc.Buffer.NumElements              = vertexCount;
    srvDesc.Buffer.StructureByteStride      = vertexStride;
    srvDesc.Buffer.Flags                    = D3D12_BUFFER_SRV_FLAG_NONE;

    Global::device->GetDevice()->CreateShaderResourceView(_skinnedVertexBuffer.Get(), &srvDesc,
                                                          _skinnedVertexBufferSRV.CPU);
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _skinnedVertexBufferUAV);
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format                           = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement              = 0;
    uavDesc.Buffer.NumElements               = vertexCount;
    uavDesc.Buffer.StructureByteStride       = vertexStride;
    uavDesc.Buffer.CounterOffsetInBytes      = 0;
    uavDesc.Buffer.Flags                     = D3D12_BUFFER_UAV_FLAG_NONE;

    Global::device->GetDevice()->CreateUnorderedAccessView(_skinnedVertexBuffer.Get(), nullptr, &uavDesc,
                                                           _skinnedVertexBufferUAV.CPU);
}

