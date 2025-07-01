#include "pch.h"
#include "AccelerationStructureManager.h"
#include "BaseMesh.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "d3dUtil.h"
// void AccelerationStructureManager::Initialize(ID3D12Device5* device)
//{
//     _topLevelBuffers = std::make_shared<AccelerationStructureBuffers>();
//
//     D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{};
//     inputs.Type        = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
//     inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
//     inputs.NumDescs    = _maxInstanceCount;
//
//       // Dummy instance desc GPU 주소 (nullptr 대체용)
//     inputs.InstanceDescs = 0;
//
//     D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo{};
//     device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);
//
//     UmDevice.CreateDefaultBuffer(static_cast<UINT>(prebuildInfo.ScratchDataSizeInBytes),
//     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
//                                  D3D12_RESOURCE_STATE_UNORDERED_ACCESS, _topLevelBuffers->pScratch);
//     UmDevice.CreateDefaultBuffer(static_cast<UINT> (prebuildInfo.ResultDataMaxSizeInBytes),
//                                  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
//                                  D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pScratch);
//     UmDevice.CreateUploadBuffer(static_cast<UINT> (prebuildInfo.ScratchDataSizeInBytes), D3D12_RESOURCE_FLAG_NONE,
//                                  D3D12_RESOURCE_STATE_GENERIC_READ, _topLevelBuffers->pInstanceDesc);
// }
//
// void AccelerationStructureManager::AddBottomLevelAS(ID3D12Device5* pDevice, ID3D12GraphicsCommandList4* cmdList,
//                                                     const MeshRenderer* mesh)
//{
//     auto& meshes = mesh->GetModel()->GetMeshes();
//     for (auto& mesh : meshes)
//     {
//        auto vibuffer = mesh->GetVIBuffer();
//         std::shared_ptr<AccelerationStructureBuffers> asbuffer = std::make_shared<AccelerationStructureBuffers>();
//        vibuffer->MakeAccelerationBuffer(pDevice, cmdList, asbuffer.get());
//         _blasList.push_back(asbuffer);
//     }
// }
//
// void AccelerationStructureManager::BuildTopLevelAS(ID3D12Device5* pDevice, ID3D12GraphicsCommandList4* cmdList,
//                                                    std::vector<Matrix> worldTransform)
//{
// }

void AccelerationStructureManager::Initialize(UINT maxInstance)
{
    ID3D12Device5* device = UmDevice.GetQueryDevice<ID3D12Device5>();
    _maxInstanceCount = maxInstance;
    _topLevelBuffers  = std::make_shared<AccelerationStructureBuffers>();

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
                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, _topLevelBuffers->pScratch);
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
    _pendingInstances.push_back(
        {renderer, _nextInstanceID++, 0, D3D12_RAYTRACING_INSTANCE_FLAG_NONE,
         renderer->GetType() == MeshRenderType::SKELETAL ? AsBuildClass::SkleltalBLAS : AsBuildClass::StaticBLAS});
}

void AccelerationStructureManager::EndFrame() 
{
}

void AccelerationStructureManager::RemoveUnUsedStaticMeshes(const std::vector<const MeshRenderer*>& liveStatics) {}

void AccelerationStructureManager::BuildOrUpdateStaticBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList,
                                                           const MeshRenderer* renderer, BlasCache& cache)
{
}

void AccelerationStructureManager::BuildDynamicBLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList,
                                                    const MeshRenderer*                            renderer,
                                                    std::shared_ptr<AccelerationStructureBuffers>& outBuf)
{
}

void AccelerationStructureManager::BuildOrUpdateTLAS(ID3D12Device5* device, ID3D12GraphicsCommandList4* cmdList) {}
