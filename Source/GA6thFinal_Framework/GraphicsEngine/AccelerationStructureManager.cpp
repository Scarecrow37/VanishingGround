#include "pch.h"
#include "AccelerationStructureManager.h"
#include "BaseMesh.h"
#include "DXRSkeletalMesh.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "Structs.h"
#include "UnorderedAccessView.h"
#include "VIBuffer.h"
#include "d3dUtil.h"
#include "RenderScene.h"

void AccelerationStructureManager::Initialize(UINT maxInstance, RenderScene* ownerScene)
{
    ComPtr<ID3D12Device5> device = Global::device->GetDevice5();
    _maxInstanceCount            = maxInstance;
    _topLevelBuffers             = std::make_shared<AccelerationStructureBuffers>();

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.Type        = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs    = _maxInstanceCount;

    // Dummy instance desc GPU 주소 (nullptr 대체용)
    inputs.InstanceDescs = 0;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo{};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

    Global::device->CreateDefaultBuffer(static_cast<UINT>(prebuildInfo.ScratchDataSizeInBytes),
                                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
                                        _topLevelBuffers->pScratch);
    Global::device->CreateDefaultBuffer(
        static_cast<UINT>(prebuildInfo.ResultDataMaxSizeInBytes), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pResult);
    Global::device->CreateUploadBuffer(static_cast<UINT>(prebuildInfo.ScratchDataSizeInBytes), D3D12_RESOURCE_FLAG_NONE,
                                       D3D12_RESOURCE_STATE_GENERIC_READ, _topLevelBuffers->pInstanceDesc);
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _topLevelBuffersSRV);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc          = {};
    srvDesc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
    srvDesc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.RaytracingAccelerationStructure.Location = _topLevelBuffers->pResult->GetGPUVirtualAddress();

    Global::device->GetDevice()->CreateShaderResourceView(nullptr, &srvDesc, _topLevelBuffersSRV.CPU);

    _ownerScene = ownerScene;
}

void AccelerationStructureManager::BeginFrame()
{
    _pendingInstances.clear();
    _nextInstanceID = 0;
}

void AccelerationStructureManager::SubmitStaticInstance(MeshInfo& meshInfo)
{
    // MeshInstanceDesc의 TransposeWorldMatrix는 포인터이므로, 객체의 주소를 전달합니다.
    _pendingInstances.push_back({meshInfo, _nextInstanceID++,
                                 0, // hitGroup
                                 D3D12_RAYTRACING_INSTANCE_FLAG_NONE, AsBuildClass::STATICBLAS});
}

void AccelerationStructureManager::SubmitSkeletalInstance(MeshInfo& meshInfo)
{
    // MeshInstanceDesc의 TransposeWorldMatrix는 포인터이므로, 객체의 주소를 전달합니다.
    _pendingInstances.push_back({
        meshInfo,
        _nextInstanceID++,
        0, // hitGroup
        D3D12_RAYTRACING_INSTANCE_FLAG_NONE,
        AsBuildClass::SKELETALBLAS,
    });
}

void AccelerationStructureManager::EndFrame(ID3D12GraphicsCommandList4* cmdList)
{
    ComPtr<ID3D12Device5> device = Global::device->GetDevice5();

    for (auto& inst : _pendingInstances)
    {
        if (AsBuildClass::STATICBLAS == inst.BuildClass)
        {
            auto& mesh  = inst.meshInfo.Mesh;
            auto& cache = _staticBlasMap[mesh];

            BuildOrUpdateStaticBLAS(device.Get(), cmdList, mesh, cache);
        }
        else
        {
            auto&                                         meshInfo = inst.meshInfo;
            auto*                                         instance = meshInfo.SkinnedInstance;
            if (instance ==nullptr)
                continue;
            uint64_t                                      instanceID = instance->GetInstanceID();
            auto&                                         chache        = _dynamicBlasMap[instanceID];
            BuildDynamicBLAS(device.Get(), cmdList, &meshInfo, chache);
        }
    }

    BuildOrUpdateTLAS(device.Get(), cmdList);
}

void AccelerationStructureManager::RemoveUnUsedStaticMeshes(const std::vector<MeshInfo>& liveStatics,
                                                            const std::vector<MeshInfo>& liveDynamics)
{
    // --- Static BLAS 제거 ---
    {
        std::unordered_set<BaseMesh*> liveStaticMeshes;
        for (const auto& meshInfo : liveStatics)
        {
            liveStaticMeshes.insert(meshInfo.Mesh);
        }

        for (auto iter = _staticBlasMap.begin(); iter != _staticBlasMap.end();)
        {
            if (liveStaticMeshes.contains(iter->first))
                ++iter;
            else
                iter = _staticBlasMap.erase(iter);
        }
    }

    // --- Dynamic BLAS 제거 ---
    {
        std::unordered_set<uint64_t> liveDynamicIDs;
        for (const auto& meshInfo : liveDynamics)
        {
            if (meshInfo.SkinnedInstance)
                liveDynamicIDs.insert(meshInfo.SkinnedInstance->GetInstanceID());
        }

        for (auto iter = _dynamicBlasMap.begin(); iter != _dynamicBlasMap.end();)
        {
            if (liveDynamicIDs.contains(iter->first))
                ++iter;
            else
                iter = _dynamicBlasMap.erase(iter);
        }
    }
}

void AccelerationStructureManager::BuildOrUpdateStaticBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList,
                                                           BaseMesh* mesh, BlasCache& cache)
{
    if (cache.buf)
    {
        return; // 이미 생성함
    }
    VIBuffer*                      viBuf = mesh->GetVIBuffer();
    D3D12_RAYTRACING_GEOMETRY_DESC geodesc{};
    geodesc.Type  = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geodesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    geodesc.Triangles.VertexBuffer.StartAddress =
        viBuf->_vertexBuffer->GetGPUVirtualAddress() + offsetof(StaticMeshVertex, Position);
    geodesc.Triangles.VertexBuffer.StrideInBytes = sizeof(StaticMeshVertex);
    geodesc.Triangles.VertexCount                = viBuf->_vertexCount;
    geodesc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
    geodesc.Triangles.IndexBuffer                = viBuf->_indexBuffer->GetGPUVirtualAddress();
    geodesc.Triangles.IndexCount                 = viBuf->_indexCount;
    geodesc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs       = 1;
    inputs.pGeometryDescs = &geodesc;
    inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    cache.buf = std::make_shared<AccelerationStructureBuffers>();
    Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
                                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
                                        cache.buf->pScratch);
    Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ResultDataMaxSizeInBytes),
                                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, cache.buf->pResult);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
    desc.Inputs                           = inputs;
    desc.ScratchAccelerationStructureData = cache.buf->pScratch->GetGPUVirtualAddress();
    desc.DestAccelerationStructureData    = cache.buf->pResult->GetGPUVirtualAddress();

    cmdList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::UAV(cache.buf->pResult.Get());
    cmdList->ResourceBarrier(1, &br);
}

void AccelerationStructureManager::BuildDynamicBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList,
                                                    MeshInfo* meshInfo, BlasCache& cache)
{
    auto&     instance = meshInfo->SkinnedInstance;
    VIBuffer* viBuf    = instance->GetVIBuffer();

    D3D12_RAYTRACING_GEOMETRY_DESC geodesc{};
    geodesc.Type  = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geodesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    geodesc.Triangles.VertexBuffer.StartAddress =
        instance->GetUpdateVertexBuffer()->GetGPUVirtualAddress() + offsetof(StaticMeshVertex, Position);
    geodesc.Triangles.VertexBuffer.StrideInBytes = sizeof(StaticMeshVertex);
    geodesc.Triangles.VertexCount                = viBuf->_vertexCount;
    geodesc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
    geodesc.Triangles.IndexBuffer                = viBuf->_indexBuffer->GetGPUVirtualAddress();
    geodesc.Triangles.IndexCount                 = viBuf->_indexCount;
    geodesc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs       = 1;
    inputs.pGeometryDescs = &geodesc;

    const bool isUpdate = (cache.buf != nullptr && cache.buf->pResult);

    inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE |
                   D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

    if (isUpdate)
        inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info{};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    if (!isUpdate)
    {
        cache.buf = std::make_shared<AccelerationStructureBuffers>();

        Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
                                            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                            D3D12_RESOURCE_STATE_COMMON, cache.buf->pScratch);

        Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ResultDataMaxSizeInBytes),
                                            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                            D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, cache.buf->pResult);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
    desc.Inputs                           = inputs;
    desc.ScratchAccelerationStructureData = cache.buf->pScratch->GetGPUVirtualAddress();
    desc.DestAccelerationStructureData    = cache.buf->pResult->GetGPUVirtualAddress();
    desc.SourceAccelerationStructureData  = isUpdate ? cache.buf->pResult->GetGPUVirtualAddress() : 0;

    cmdList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

    CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(cache.buf->pResult.Get());
    cmdList->ResourceBarrier(1, &uavBarrier);
}

void AccelerationStructureManager::BuildOrUpdateTLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList)
{
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> inst;
    inst.reserve(_pendingInstances.size());

    for (const auto& p : _pendingInstances)
    {
        Matrix*                      transoseWorld = &_ownerScene->_matrices[p.meshInfo.InstanceData.MatrixID].World;
        D3D12_RAYTRACING_INSTANCE_DESC desc{};
        memcpy(desc.Transform, transoseWorld, sizeof(desc.Transform));
        desc.InstanceID                          = p.InstanceID;
        desc.InstanceContributionToHitGroupIndex = p.HitGroupIndex;
        desc.InstanceMask                        = 0xFF;
        desc.Flags                               = p.Flags;

        if (p.BuildClass == AsBuildClass::STATICBLAS)
        {
            auto it = _staticBlasMap.find(p.meshInfo.Mesh);
            if (it != _staticBlasMap.end() && it->second.buf)
            {
                desc.AccelerationStructure = it->second.buf->pResult->GetGPUVirtualAddress();
            }
        }
        else if (p.BuildClass == AsBuildClass::SKELETALBLAS)
        {
            uint64_t instanceID = p.meshInfo.SkinnedInstance->GetInstanceID();
            auto     it         = _dynamicBlasMap.find(instanceID);
            if (it != _dynamicBlasMap.end() && it->second.buf)
            {
                desc.AccelerationStructure = it->second.buf->pResult->GetGPUVirtualAddress();
            }
        }

        inst.push_back(desc);
    }

    const UINT instCount    = static_cast<UINT>(inst.size());
    const UINT instByteSize = instCount * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);

    if (instCount > 0)
    {
        if (!_instanceUpload || _instanceUpload->GetDesc().Width < instByteSize)
        {
            Global::device->CreateUploadBuffer(instByteSize, D3D12_RESOURCE_FLAG_NONE,
                                               D3D12_RESOURCE_STATE_GENERIC_READ, _instanceUpload);
        }

        void* data = nullptr;
        _instanceUpload->Map(0, nullptr, &data);
        memcpy(data, inst.data(), instByteSize);
        _instanceUpload->Unmap(0, nullptr);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs      = instCount;
    inputs.InstanceDescs = (instCount > 0) ? _instanceUpload->GetGPUVirtualAddress() : 0;
    inputs.Flags         = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info{};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    if (_topLevelBuffers->pScratch->GetDesc().Width < info.ScratchDataSizeInBytes)
    {
        Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
                                            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
                                            _topLevelBuffers->pScratch);

        Global::device->CreateDefaultBuffer(
            static_cast<UINT>(info.ResultDataMaxSizeInBytes), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pResult);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
        srvDesc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.RaytracingAccelerationStructure.Location = _topLevelBuffers->pResult->GetGPUVirtualAddress();

        Global::device->GetDevice()->CreateShaderResourceView(nullptr, &srvDesc, _topLevelBuffersSRV.CPU);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
    desc.Inputs                           = inputs;
    desc.ScratchAccelerationStructureData = _topLevelBuffers->pScratch->GetGPUVirtualAddress();
    desc.DestAccelerationStructureData    = _topLevelBuffers->pResult->GetGPUVirtualAddress();

    cmdList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::UAV(_topLevelBuffers->pResult.Get());
    cmdList->ResourceBarrier(1, &br);
}
