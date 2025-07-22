#include "pch.h"
#include "DXRComputeSkeletalMeshPass.h"
#include "ShaderBuilder.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "BaseMesh.h"
#include "Structs.h"
#include "RenderScene.h"
#include "FrameResource.h"

DXRComputeSkeletalMeshPass::~DXRComputeSkeletalMeshPass() {}

void DXRComputeSkeletalMeshPass::Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, commandList);
    InitShaderAndPSO();
}

void DXRComputeSkeletalMeshPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pso.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
}

void DXRComputeSkeletalMeshPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    for (auto& [isDestroy, component] : _ownerScene->_meshRenderQueue)
    {
        if (!component->IsActive() || component->GetType() != MeshRenderType::SKELETAL)
            continue;

        Dispatch(commandList, component);
    }
}

void DXRComputeSkeletalMeshPass::End(ID3D12GraphicsCommandList* commandList) {}

void DXRComputeSkeletalMeshPass::Dispatch(ID3D12GraphicsCommandList* commandList, MeshRenderer* renderer)
{
    const auto& model = renderer->GetModel();
    if (!model)
        return;

    const auto& meshes            = model->GetMeshes();
    auto& skeletalInstances = renderer->GetSkeletaMesheInstances();

    UINT instanceID = 0; // This needs to be correctly identified from the renderer

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        const auto& mesh = meshes[i];
        auto& instance = skeletalInstances[i];

        // Transition UAV to be writable
        instance->UAVBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        commandList->SetComputeRootUnorderedAccessView(_shader->GetRootParameterIndex("skinnedVertices"), instance->UAVBuffer->GetGPUVirtualAddress());
        commandList->SetComputeRootShaderResourceView(_shader->GetRootParameterIndex("vertices"), mesh->GetVIBuffer()->_vertexBuffer->GetGPUVirtualAddress());
        //commandList->SetComputeRootShaderResourceView(_shader->GetRootParameterIndex("skinningInfo"), mesh->GetVIBuffer()->_skinningBuffer->GetGPUVirtualAddress());

        UINT objectData[2] = { instanceID, MAX_BONE_MATRIX };
        commandList->SetComputeRoot32BitConstants(_shader->GetRootParameterIndex("objectData"), 2, objectData, 0);

        UINT threadGroupsX = (mesh->GetVIBuffer()->_vertexCount + 255) / 256;
        commandList->Dispatch(threadGroupsX, 1, 1);

        // Transition UAV to be readable for subsequent passes
        instance->UAVBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    }
}

void DXRComputeSkeletalMeshPass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_skeletal_skinning.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = _shader->GetRootSignature();
    psoDesc.CS = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr = Global::device->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_pso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DXRComputeSkeletalMeshPass::InitShaderAndPSO CreateComputePipelineState failed");
}