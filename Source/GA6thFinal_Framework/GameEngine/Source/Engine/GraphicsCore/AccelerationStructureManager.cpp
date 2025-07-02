#include "pch.h"
#include "AccelerationStructureManager.h"
#include "BaseMesh.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "VIBuffer.h"
#include "d3dUtil.h"

void AccelerationStructureManager::Initialize(UINT maxInstance)
{
    ComPtr<ID3D12Device5> device = UmDevice.GetDevice5();
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

    UmDevice.CreateDefaultBuffer(static_cast<UINT>(prebuildInfo.ScratchDataSizeInBytes),
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
                                 _topLevelBuffers->pScratch);
    UmDevice.CreateDefaultBuffer(static_cast<UINT>(prebuildInfo.ResultDataMaxSizeInBytes),
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pResult);
    UmDevice.CreateUploadBuffer(static_cast<UINT>(prebuildInfo.ScratchDataSizeInBytes), D3D12_RESOURCE_FLAG_NONE,
                                D3D12_RESOURCE_STATE_GENERIC_READ, _topLevelBuffers->pInstanceDesc);
}

void AccelerationStructureManager::BeginFrame()
{
    _pendingInstances.clear();
    _dynamicBlas.clear();
}

void AccelerationStructureManager::SubmitInstance(const MeshRenderer* renderer)
{
    if (!renderer->GetModel())
        return;
    _pendingInstances.push_back(
        {renderer, _nextInstanceID++,
         0, // hitGroup
         D3D12_RAYTRACING_INSTANCE_FLAG_NONE,
         renderer->GetType() == MeshRenderType::SKELETAL ? AsBuildClass::SKELETALBLAS : AsBuildClass::STATICBLAS});
}

void AccelerationStructureManager::EndFrame()
{
    ComPtr<ID3D12Device5>              device  = UmDevice.GetDevice5();
    ComPtr<ID3D12GraphicsCommandList4> cmdList = UmDevice.GetCommandList4();
    for (auto& inst : _pendingInstances)
    {
        if (AsBuildClass::STATICBLAS == inst.BuildClass)
        {
            const Model* model = inst.Renderer->GetModel().get();
            auto&        cache = _staticBlasMap[model];
            BuildOrUpdateStaticBLAS(device.Get(), cmdList.Get(), inst.Renderer, cache);
        }
        else
        {
            std::shared_ptr<AccelerationStructureBuffers> buf;
            BuildDynamicBLAS(device.Get(), cmdList.Get(), inst.Renderer, buf);
            _dynamicBlas.push_back(buf);
        }
    }

    BuildOrUpdateTLAS(device.Get(), cmdList.Get());
}

void AccelerationStructureManager::RemoveUnUsedStaticMeshes(const std::vector<MeshRenderer*>& liveStatics)
{
    std::unordered_set<const Model*> live;
    for (auto* renderer : liveStatics)
    {
        live.insert(renderer->GetModel().get());
    }
    for (auto iter = _staticBlasMap.begin(); iter != _staticBlasMap.end();)
    {
        if (live.contains(iter->first))
            ++iter;
        else
            iter = _staticBlasMap.erase(iter);
    }
}

void AccelerationStructureManager::BuildOrUpdateStaticBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList,
                                                           const MeshRenderer* renderer, BlasCache& cache)
{
    if (cache.buf)
    {
        cache.refCount++;
        return; // 이미 생성함
    }
    std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geos;
    const auto&                                 meshes = renderer->GetModel()->GetMeshes();
    geos.reserve(meshes.size());

    for (auto& mesh : meshes)
    {
        BaseMesh*                      m     = mesh.get();
        VIBuffer*                      viBuf = m->GetVIBuffer();
        D3D12_RAYTRACING_GEOMETRY_DESC desc{};
        desc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        desc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        desc.Triangles.VertexBuffer.StartAddress  = viBuf->_vertexBuffer->GetGPUVirtualAddress()+offsetof(Vertex,Position);
        desc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);
        desc.Triangles.VertexCount                = viBuf->_vertexCount;
        desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
        desc.Triangles.IndexBuffer                = viBuf->_indexBuffer->GetGPUVirtualAddress();
        desc.Triangles.IndexCount                 = viBuf->_indexCount;
        desc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;
        geos.push_back(desc);
    }
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs       = static_cast<UINT>(geos.size());
    inputs.pGeometryDescs = geos.data();
    inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    cache.buf = std::make_shared<AccelerationStructureBuffers>();
    UmDevice.CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
                                 cache.buf->pScratch);
    UmDevice.CreateDefaultBuffer(static_cast<UINT>(info.ResultDataMaxSizeInBytes),
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, cache.buf->pResult);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
    desc.Inputs = inputs;
    desc.ScratchAccelerationStructureData = cache.buf->pScratch->GetGPUVirtualAddress();
    desc.DestAccelerationStructureData = cache.buf->pResult->GetGPUVirtualAddress();
    CD3DX12_RESOURCE_BARRIER br           = CD3DX12_RESOURCE_BARRIER::UAV(cache.buf->pResult.Get());
    cmdList->ResourceBarrier(1, &br);
    cache.refCount=1;
}


void AccelerationStructureManager::BuildDynamicBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList,
                                                    const MeshRenderer*                            renderer,
                                                    std::shared_ptr<AccelerationStructureBuffers>& outBuf)
{
    //bool firstBuild = (outBuf == nullptr);

    //// ── GeometryDesc : 스키닝 결과 VertexBuffer (ComputeShader 등으로 생성) ─
    //auto                           vb = renderer->GetModel()->GetSkinnedVertexBuffer(); // 구현체에 맞게 호출
    //D3D12_RAYTRACING_GEOMETRY_DESC g{};
    //g.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    //g.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    //g.Triangles.VertexBuffer.StartAddress  = vb.gpuVA;
    //g.Triangles.VertexBuffer.StrideInBytes = vb.stride;
    //g.Triangles.VertexCount                = vb.count;
    //g.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
    //g.Triangles.IndexBuffer                = vb.idxGpuVA;
    //g.Triangles.IndexCount                 = vb.idxCount;
    //g.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

    //D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS in{};
    //in.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    //in.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
    //in.NumDescs       = 1;
    //in.pGeometryDescs = &g;
    //in.Flags =
    //    firstBuild
    //        ? (D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE |
    //           D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE)
    //        : (D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE |
    //           D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
    //           D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE); // 업데이트 플래그
    //                                                                               // :contentReference[oaicite:0]{index=0}

    //D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info{};
    //device->GetRaytracingAccelerationStructurePrebuildInfo(&in, &info);

    //if (firstBuild)
    //{
    //    outBuf = std::make_shared<AccelerationStructureBuffers>();
    //    UmDevice.CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
    //                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
    //                                 outBuf->pScratch);

    //    UmDevice.CreateDefaultBuffer(static_cast<UINT>(info.ResultDataMaxSizeInBytes),
    //                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
    //                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, outBuf->pResult);
    //}

    //// ── Build/Update 명령 ────────────────────────────────
    //D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
    //desc.Inputs                           = in;
    //desc.ScratchAccelerationStructureData = outBuf->pScratch->GetGPUVirtualAddress();
    //desc.DestAccelerationStructureData    = outBuf->pResult->GetGPUVirtualAddress();
    //desc.SourceAccelerationStructureData  = firstBuild ? 0 : outBuf->pResult->GetGPUVirtualAddress();

    //cmdList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr); // :contentReference[oaicite:1]{index=1}
}

void AccelerationStructureManager::BuildOrUpdateTLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList) 
{
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> inst;
    inst.reserve(_pendingInstances.size());

    size_t dynIdx = 0;
    for (const auto& p : _pendingInstances)
    {
        const Matrix& world = p.Renderer->GetWorldMatrix();
        XMFLOAT3X4    m3x4;
        XMStoreFloat3x4(&m3x4, XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&world))));
        
        D3D12_RAYTRACING_INSTANCE_DESC desc{};
        memcpy(desc.Transform, &m3x4, sizeof(desc.Transform));
        desc.InstanceContributionToHitGroupIndex = p.InstanceID;
        desc.InstanceContributionToHitGroupIndex = p.HitGroupIndex;
        desc.InstanceMask                        = 0xFF;
        desc.Flags                               = p.Flags;

        if (p.BuildClass == AsBuildClass::STATICBLAS)
        {
            Model* model = p.Renderer->GetModel().get();
            desc.AccelerationStructure = _staticBlasMap[model].buf->pResult->GetGPUVirtualAddress();
        }
        else
        {
            desc.AccelerationStructure = _dynamicBlas[dynIdx++]->pResult->GetGPUVirtualAddress();
        }
        inst.push_back(desc);
    }
    
    const UINT instByteSize = static_cast<UINT>(inst.size() * sizeof(D3D12_RAYTRACING_INSTANCE_DESC));
    if (instByteSize == 0)
        return;
    if (!_instanceUpload || _instanceUpload->GetDesc().Width < instByteSize)
    {
        UmDevice.CreateUploadBuffer(instByteSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ,
                                    _instanceUpload);
    }
    
    void* data = nullptr;
    _instanceUpload->Map(0, nullptr, &data);
    memcpy(data, inst.data(), instByteSize);
    _instanceUpload->Unmap(0, nullptr);
    
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs    = static_cast<UINT>(inst.size());
    inputs.InstanceDescs = _instanceUpload->GetGPUVirtualAddress();
    inputs.Flags         = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
    
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs,&info);
    
    if (_topLevelBuffers->pScratch->GetDesc().Width < info.ScratchDataSizeInBytes)
    {
        // 기존 버퍼 크기가 부족하기에 새로 만들어줌
        UmDevice.CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
                                     _topLevelBuffers->pScratch);

        UmDevice.CreateDefaultBuffer(static_cast<UINT>(info.ResultDataMaxSizeInBytes),
                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                     D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pResult);
    }
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
    desc.Inputs = inputs;
    desc.ScratchAccelerationStructureData = _topLevelBuffers->pScratch->GetGPUVirtualAddress();
    desc.DestAccelerationStructureData    = _topLevelBuffers->pResult->GetGPUVirtualAddress();

    cmdList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
}
    