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
    commandList->SetPipelineState(_pso.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
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
    auto& skeletalInstances = meshInfo.SkinnedInstance;

    ID3D12Resource* resource = skeletalInstances->GetUpdateVertexBuffer();
    VIBuffer*       vibuffer = skeletalInstances->GetVIBuffer();
    auto            br = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
                                                              D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    auto&           frameResource = _ownerScene->_frameResources[_ownerScene->_currentFrameIndex];
    commandList->ResourceBarrier(1, &br);
    commandList->SetComputeRootUnorderedAccessView(_shader->GetRootParameterIndex("skinnedVertices"),
                                                   resource->GetGPUVirtualAddress());
    commandList->SetComputeRootShaderResourceView(_shader->GetRootParameterIndex("vertices"),
                                                  vibuffer->_vertexBuffer->GetGPUVirtualAddress());
    frameResource->SetComputeFrameResource(FrameResourceType::BONE_MATRICES,
                                           _shader->GetRootParameterIndex("boneMatrices"), commandList);

    UINT parameter[3]{0, MAX_BONE_MATRIX, 0};
    parameter[0] = meshInfo.InstanceID;
    parameter[2] = vibuffer->_vertexCount;
    commandList->SetComputeRoot32BitConstants(_shader->GetRootParameterIndex("bit32_4_objectData"), 3, parameter, 0);

    UINT threadGroupsX = (vibuffer->_vertexCount + 255) / 256;
    commandList->Dispatch(threadGroupsX, 1, 1);
    br = CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                              D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void DXRComputeSkeletalMeshPass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_skeletal_skinning.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature                    = _shader->GetRootSignature();
    psoDesc.CS                                = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psoDesc.Flags                             = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr = Global::device->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_pso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRComputeSkeletalMeshPass::InitShaderAndPSO CreateComputePipelineState failed");
}