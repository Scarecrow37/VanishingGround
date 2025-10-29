#include "pch.h"
#include "DXRComputeSkeletalMeshPass.h"
#include "BaseMesh.h"
#include "DXRSkeletalMesh.h"
#include "FrameResource.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "RenderScene.h"
#include "ShaderBuilder.h"
#include "Structs.h"

DXRComputeSkeletalMeshPass::~DXRComputeSkeletalMeshPass() = default;

void DXRComputeSkeletalMeshPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
}

void DXRComputeSkeletalMeshPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_fx.GetRootSignature());
}

void DXRComputeSkeletalMeshPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    // auto computeCmdList = Global::device->GetComputeCommandList();
    for (auto& meshInfo : _ownerScene->_activeMeshes[SKELETAL_MESH])
    {
        Dispatch(commandList, meshInfo);
    }
}

void DXRComputeSkeletalMeshPass::End(ID3D12GraphicsCommandList* commandList) {}

void DXRComputeSkeletalMeshPass::Dispatch(ID3D12GraphicsCommandList* commandList, MeshInfo meshInfo)
{
    if (nullptr == meshInfo.SkinnedInstance)
        return;

    auto* skeletalInstance = meshInfo.SkinnedInstance;
    VIBuffer* viBuffer         = skeletalInstance->GetVIBuffer();
    ID3D12Resource* skinnedVertexBuffer = skeletalInstance->GetUpdateVertexBuffer();
    auto br = CD3DX12_RESOURCE_BARRIER::Transition(skinnedVertexBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
                                                   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    commandList->ResourceBarrier(1, &br);

    auto& frameResource = _ownerScene->_frameResources[_ownerScene->_currentFrameIndex];
    
    commandList->SetComputeRootUnorderedAccessView(_fx.GetRootParameterIndex("skinnedVertices"),
                                                   skinnedVertexBuffer->GetGPUVirtualAddress());
    commandList->SetComputeRootShaderResourceView(_fx.GetRootParameterIndex("vertices"),
                                                  viBuffer->_vertexBuffer->GetGPUVirtualAddress());
    frameResource->SetComputeFrameResource(FrameResourceType::BONE_MATRICES, _fx.GetRootParameterIndex("boneMatrices"),
                                           commandList);

    MeshData meshData;
    meshData.InstanceID = meshInfo.InstanceData.MatrixID;
    meshData.BoneMatrixOffset = MAX_BONE_MATRIX;
    meshData.VertexCount      = viBuffer->_vertexCount;
    
    commandList->SetComputeRoot32BitConstants(_fx.GetRootParameterIndex("bit32_3_meshData"), 3, &meshData, 0);
    
    UINT threadGroupsX = (viBuffer->_vertexCount + 255) / 256;
    commandList->Dispatch(threadGroupsX, 1, 1);
    
    br = CD3DX12_RESOURCE_BARRIER::Transition(skinnedVertexBuffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                              D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);
}

void DXRComputeSkeletalMeshPass::InitShaderAndPSO()
{
    ComputePipelineStateStream pss;
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);    
}