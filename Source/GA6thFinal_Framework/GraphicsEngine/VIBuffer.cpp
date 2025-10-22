#include "pch.h"
#include "VIBuffer.h"
#include "d3dUtil.h"

void VIBuffer::Initialize(const VIBuffer::Descriptor& descriptor)
{
    ID3D12Device* device = Global::device->GetDevice();
    Global::device->CreateVertexBuffer(descriptor.vertexData, 
								descriptor.vertexSize, 
								descriptor.vertexStride, 
								_vertexBuffer,
								_vertexBufferView);

	Global::device->CreateIndexBuffer(descriptor.indexData, 
							   descriptor.indexSize, 
							   DXGI_FORMAT_R32_UINT,
							   _indexBuffer,
							   _indexBufferView);

	_indexCount = descriptor.indexCount;
    _vertexCount = descriptor.vertexSize/descriptor.vertexStride;

    if (Global::isRayTracing)
    {
        _vertexBufferID = Global::viewManager->GetNumVertexBuffer();
        _indexBufferID  = Global::viewManager->GetNumIndexBuffer();
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::VERTEX_BUFFER_SHADER_RESOURCE, _vertexBufferSrv);
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::INDEX_BUFFER_SHADER_RESOURCE, _indexBufferSrv);


        D3D12_SHADER_RESOURCE_VIEW_DESC srvd{};
        srvd.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
        srvd.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvd.Format                     = DXGI_FORMAT_UNKNOWN;
        srvd.Buffer.NumElements         = _vertexCount;
        srvd.Buffer.StructureByteStride = descriptor.vertexStride;
        srvd.Buffer.FirstElement        = 0;
        device->CreateShaderResourceView(_vertexBuffer.Get(), &srvd, _vertexBufferSrv.CPU);

        srvd.Buffer.NumElements         = _indexCount;
        srvd.Buffer.StructureByteStride = sizeof(UINT);
        device->CreateShaderResourceView(_indexBuffer.Get(), &srvd, _indexBufferSrv.CPU);
    }
}

void VIBuffer::Initialize(ID3D12GraphicsCommandList* commandList, const VIBuffer::Descriptor& descriptor)
{
    ID3D12Device* device = Global::device->GetDevice();
    Global::device->CreateVertexBuffer(commandList, 
                                       descriptor.vertexData, 
                                       descriptor.vertexSize,
                                       descriptor.vertexStride, 
                                       _vertexBuffer, 
                                       _vertexBufferView);

    Global::device->CreateIndexBuffer(commandList, 
                                      descriptor.indexData, 
                                      descriptor.indexSize, 
                                      DXGI_FORMAT_R32_UINT, 
                                      _indexBuffer, 
                                      _indexBufferView);

    _indexCount  = descriptor.indexCount;
    _vertexCount = descriptor.vertexSize / descriptor.vertexStride;

    if (Global::isRayTracing)
    {
        _vertexBufferID = Global::viewManager->GetNumVertexBuffer();
        _indexBufferID  = Global::viewManager->GetNumIndexBuffer();
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::VERTEX_BUFFER_SHADER_RESOURCE, _vertexBufferSrv);
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::INDEX_BUFFER_SHADER_RESOURCE, _indexBufferSrv);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvd{};
        srvd.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
        srvd.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvd.Format                     = DXGI_FORMAT_UNKNOWN;
        srvd.Buffer.NumElements         = _vertexCount;
        srvd.Buffer.StructureByteStride = descriptor.vertexStride;
        srvd.Buffer.FirstElement        = 0;
        device->CreateShaderResourceView(_vertexBuffer.Get(), &srvd, _vertexBufferSrv.CPU);

        srvd.Buffer.NumElements         = _indexCount;
        srvd.Buffer.StructureByteStride = sizeof(UINT);
        device->CreateShaderResourceView(_indexBuffer.Get(), &srvd, _indexBufferSrv.CPU);
    }
}

void VIBuffer::DrawIndexedInstanced(ID3D12GraphicsCommandList* commandList, UINT instanceCount)
{
	commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	commandList->IASetIndexBuffer(&_indexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexedInstanced(_indexCount, instanceCount,  0, 0, 0);
}

void VIBuffer::MakeAccelerationBuffer(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList, AccelerationStructureBuffers* outBuffer)
{
    D3D12_RAYTRACING_GEOMETRY_DESC geoDesc{};
    geoDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geoDesc.Triangles.VertexBuffer.StartAddress = _vertexBuffer->GetGPUVirtualAddress();
    geoDesc.Triangles.VertexBuffer.StrideInBytes = static_cast<UINT64>(sizeof(Vertex));
    geoDesc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32A32_FLOAT;// Vertex 구조체의 맨 앞에는 position이 와야하고 그 pos를 읽는 비트수
    geoDesc.Triangles.VertexCount = _vertexCount;
    geoDesc.Triangles.IndexBuffer = _indexBuffer->GetGPUVirtualAddress();
    geoDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
    geoDesc.Triangles.IndexCount  = _indexCount;
    geoDesc.Flags                 = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs    = 1;
    inputs.pGeometryDescs = &geoDesc;
    inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO preBuildInfo{};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &preBuildInfo);

    // scratch(임시 빌드 공간)
    Global::device->CreateDefaultBuffer(static_cast<UINT> (preBuildInfo.ScratchDataSizeInBytes),
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                 D3D12_RESOURCE_STATE_UNORDERED_ACCESS, outBuffer->pScratch);
    // result(blas 저장 공간)
    Global::device->CreateDefaultBuffer(static_cast<UINT>(preBuildInfo.ResultDataMaxSizeInBytes),
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, outBuffer->pResult);
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
    asDesc.Inputs                           = inputs;
    asDesc.DestAccelerationStructureData    = outBuffer->pResult->GetGPUVirtualAddress();
    asDesc.ScratchAccelerationStructureData = outBuffer->pScratch->GetGPUVirtualAddress();
    
    cmdList->BuildRaytracingAccelerationStructure(&asDesc, 0, nullptr);

    auto br = CD3DX12_RESOURCE_BARRIER::UAV(outBuffer->pResult.Get());
    cmdList->ResourceBarrier(1, &br);
}
