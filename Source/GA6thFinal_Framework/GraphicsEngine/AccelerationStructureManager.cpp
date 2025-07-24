#include "pch.h"
#include "AccelerationStructureManager.h"
#include "BaseMesh.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "VIBuffer.h"
#include "d3dUtil.h"
#include "Structs.h"
#include "UnorderedAccessView.h"

void AccelerationStructureManager::Initialize(UINT maxInstance)
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
    Global::device->CreateDefaultBuffer(static_cast<UINT>(prebuildInfo.ResultDataMaxSizeInBytes),
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pResult);
    Global::device->CreateUploadBuffer(static_cast<UINT>(prebuildInfo.ScratchDataSizeInBytes), D3D12_RESOURCE_FLAG_NONE,
                                D3D12_RESOURCE_STATE_GENERIC_READ, _topLevelBuffers->pInstanceDesc);
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _topLevelBuffersSRV);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc          = {};
    srvDesc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
    srvDesc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.RaytracingAccelerationStructure.Location = _topLevelBuffers->pResult->GetGPUVirtualAddress();

    Global::device->GetDevice()->CreateShaderResourceView(nullptr, &srvDesc, _topLevelBuffersSRV.CPU);
}

void AccelerationStructureManager::BeginFrame()
{
    _pendingInstances.clear();
    _dynamicBlas.clear();
    _nextInstanceID = 0;
}

void AccelerationStructureManager::SubmitInstance(MeshRenderer* renderer)
{
    if (!renderer->GetModel())
        return;
    auto& meshes = renderer->GetModel()->GetMeshes();
    for (auto& key : meshes)
    {
        _pendingInstances.push_back(
            {renderer, key.get(), _nextInstanceID++,
             0, // hitGroup
             D3D12_RAYTRACING_INSTANCE_FLAG_NONE,
             renderer->GetType() == MeshRenderType::SKELETAL ? AsBuildClass::SKELETALBLAS : AsBuildClass::STATICBLAS});
    }
}

void AccelerationStructureManager::EndFrame(ID3D12GraphicsCommandList4* cmdList)
{
    ComPtr<ID3D12Device5>              device  = Global::device->GetDevice5();
    
    for (auto& inst : _pendingInstances)
    {
        if (AsBuildClass::STATICBLAS == inst.BuildClass)
        {
            auto& mesh  = inst.key;
            auto& cache = _staticBlasMap[mesh];
            //auto                               commandlist = Global::device->GetCommandList();
            //ComPtr<ID3D12GraphicsCommandList4> commandList4;
            //HRESULT hr = commandlist->QueryInterface(IID_PPV_ARGS(commandList4.GetAddressOf()));
            //FAILED_CHECK_MESSAGE(hr, L"DXRDrawPass::Draw() failed to get ID3D12GraphicsCommandList4 interface");

            BuildOrUpdateStaticBLAS(device.Get(), cmdList, mesh, cache);
    /*        commandlist->Close();
            Global::commandController->ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE, commandlist);

            Global::device->ResetGraphicsCommnad();*/
        }
        else
        {
            std::shared_ptr<AccelerationStructureBuffers> buf;
            BuildDynamicBLAS(device.Get(), cmdList, inst.Renderer, buf);
            _dynamicBlas.push_back(buf);
        }
    }

    BuildOrUpdateTLAS(device.Get(), cmdList);
}

void AccelerationStructureManager::RemoveUnUsedStaticMeshes(const std::vector<MeshRenderer*>& liveStatics)
{
    std::unordered_set<BaseMesh*> live;
    for (auto* renderer : liveStatics)
    {
        auto& model  = renderer->GetModel();
        auto& meshes = model->GetMeshes();
        for (auto& mesh : meshes)
        {
            live.insert(mesh.get());
        }
    }
    for (auto iter = _staticBlasMap.begin(); iter != _staticBlasMap.end();)
    {
        if (live.contains(iter->first))
            ++iter;
        else
        {
            iter = _staticBlasMap.erase(iter);
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
                                                    MeshRenderer*                            renderer,
                                                    std::shared_ptr<AccelerationStructureBuffers>& outBuf)
{
    const auto& model = renderer->GetModel();
    if (!model)
        return;

    const auto& meshes = model->GetMeshes();
    auto& skeletalInstances = renderer->GetSkeletaMesheInstances();

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        const auto& mesh = meshes[i];
        auto& instance = skeletalInstances[i];
        VIBuffer*   viBuf = mesh->GetVIBuffer();

        D3D12_RAYTRACING_GEOMETRY_DESC geodesc{};
        geodesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        geodesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        geodesc.Triangles.VertexBuffer.StartAddress =
            instance->UAVBuffer->GetResource()->GetGPUVirtualAddress() + offsetof(StaticMeshVertex, Position);
        geodesc.Triangles.VertexBuffer.StrideInBytes = sizeof(StaticMeshVertex);
        geodesc.Triangles.VertexCount = viBuf->_vertexCount;
        geodesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        geodesc.Triangles.IndexBuffer = viBuf->_indexBuffer->GetGPUVirtualAddress();
        geodesc.Triangles.IndexCount = viBuf->_indexCount;
        geodesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
        inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        inputs.NumDescs = 1;
        inputs.pGeometryDescs = &geodesc;
        inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
        device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

        if (outBuf == nullptr)
        {
            outBuf = std::make_shared<AccelerationStructureBuffers>();
            Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
                                         D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, outBuf->pScratch);
            Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ResultDataMaxSizeInBytes),
                                         D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                         D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, outBuf->pResult);
        }

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
        desc.Inputs = inputs;
        desc.ScratchAccelerationStructureData = outBuf->pScratch->GetGPUVirtualAddress();
        desc.DestAccelerationStructureData = outBuf->pResult->GetGPUVirtualAddress();
        desc.SourceAccelerationStructureData = (outBuf == nullptr) ? 0 : outBuf->pResult->GetGPUVirtualAddress();

        cmdList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

        CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::UAV(outBuf->pResult.Get());
        cmdList->ResourceBarrier(1, &br);
    }
}

void AccelerationStructureManager::BuildOrUpdateTLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList)
{
    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> inst;
    inst.reserve(_pendingInstances.size());

    size_t dynIdx = 0;
    for (const auto& p : _pendingInstances)
    {
        const Matrix& world         = p.Renderer->GetWorldMatrix();
        Matrix        transposWorld = world.Transpose();

        D3D12_RAYTRACING_INSTANCE_DESC desc{};
        memcpy(desc.Transform, &transposWorld, sizeof(desc.Transform));
        desc.InstanceID                          = p.InstanceID;
        desc.InstanceContributionToHitGroupIndex = p.HitGroupIndex;
        desc.InstanceMask                        = 0xFF;
        desc.Flags                               = p.Flags;

        if (p.BuildClass == AsBuildClass::STATICBLAS)
        {
            desc.AccelerationStructure = _staticBlasMap[p.key].buf->pResult->GetGPUVirtualAddress();
        }
        else
        {
            desc.AccelerationStructure = _dynamicBlas[dynIdx++]->pResult->GetGPUVirtualAddress();
        }
        inst.push_back(desc);
    }
    
    const UINT instCount    = static_cast<UINT>(inst.size());
    const UINT instByteSize = static_cast<UINT>(inst.size() * sizeof(D3D12_RAYTRACING_INSTANCE_DESC));
    if (instCount > 0)
    {
        // Upload 버퍼 크기 확인 및 재할당
        if (!_instanceUpload || _instanceUpload->GetDesc().Width < instByteSize)
        {
            Global::device->CreateUploadBuffer(instByteSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, _instanceUpload);
        }
        // CPU 데이터를 Upload 버퍼에 복사
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

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info;
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    if (_topLevelBuffers->pScratch->GetDesc().Width < info.ScratchDataSizeInBytes)
    {
        // 기존 버퍼 크기가 부족하기에 새로 만들어줌
        Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ScratchDataSizeInBytes),
                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
                                     _topLevelBuffers->pScratch);

        Global::device->CreateDefaultBuffer(static_cast<UINT>(info.ResultDataMaxSizeInBytes),
                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                     D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pResult);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc          = {};
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