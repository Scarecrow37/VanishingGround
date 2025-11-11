#include "pch.h"
#include "CustomShaderPass.h"
#include "BaseMesh.h"
#include "FrameResource.h"
#include "MeshRenderer.h"

// CustomMaterials
#include "TransparentRimLight.h"

CustomShaderPass::CustomShaderPass() = default;

CustomShaderPass::~CustomShaderPass() = default;

void CustomShaderPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    
    _customMaterials.resize(static_cast<UINT>(CustomLightType::END));

    _customMaterials[static_cast<UINT>(CustomLightType::TRANSPARENT_RIM_LIGHT)] = std::make_unique<TransparentRimLight>();
    _customMaterials[static_cast<UINT>(CustomLightType::TRANSPARENT_RIM_LIGHT)]->Initialize();
}

void CustomShaderPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("Forward G-Buffer");
    for (auto& renderTarget : renderTargetGroup)
    {
        renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[MAX_FORWARD_GROUP]{};
    rtvHandles[0] = _meshRenderTarget->GetRTVHandle();
    for (int i = 1; i < MAX_FORWARD_GROUP; i++)
    {
        rtvHandles[i] = renderTargetGroup[i - 1]->GetRTVHandle();
    }

    commandList->OMSetRenderTargets(MAX_FORWARD_GROUP, rtvHandles, FALSE, &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewport());
}

void CustomShaderPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();

    for (auto& meshInfo : _ownerScene->_activeMeshes[STATIC_MESH])
    {
        if (meshInfo.Material.ShadingModel != Material::ShadingModelType::CUSTOMLIT)
            continue;

        auto  type           = meshInfo.Renderer->GetCustomLightType();
        auto& customMaterial = _customMaterials[(UINT)type];

        if (!customMaterial)
            continue;

        customMaterial->SetMaterial(commandList, meshInfo.Renderer);
        commandList->SetGraphicsRoot32BitConstants(customMaterial->GetRootParameterIndex(STATIC_MESH, "bit32_7_instanceData"), 7, &meshInfo.InstanceData, 0);
        frameResource->SetFrameResource(FrameResourceType::TRANSFORM, customMaterial->GetRootParameterIndex(STATIC_MESH, "matrices"), commandList);

        UINT rootParameterIndex = customMaterial->GetRootParameterIndex(STATIC_MESH, "textures");
        if (rootParameterIndex != UINT_MAX)
        {
            commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, resource);
        }

        meshInfo.Mesh->Render(commandList);
    }

    for (auto& meshInfo : _ownerScene->_activeMeshes[SKELETAL_MESH])
    {
        if (meshInfo.Material.ShadingModel != Material::ShadingModelType::CUSTOMLIT)
            continue;

        auto  type           = meshInfo.Renderer->GetCustomLightType();
        auto& customMaterial = _customMaterials[(UINT)type];

        if (!customMaterial)
            continue;

        customMaterial->SetMaterial(commandList, meshInfo.Renderer);
        commandList->SetGraphicsRoot32BitConstants(customMaterial->GetRootParameterIndex(SKELETAL_MESH, "bit32_7_instanceData"), 7, &meshInfo.InstanceData, 0);
        frameResource->SetFrameResource(FrameResourceType::TRANSFORM, customMaterial->GetRootParameterIndex(SKELETAL_MESH, "matrices"), commandList);
        frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, customMaterial->GetRootParameterIndex(SKELETAL_MESH, "boneMatrices"), commandList);

        UINT rootParameterIndex = customMaterial->GetRootParameterIndex(SKELETAL_MESH, "textures");
        if (rootParameterIndex != UINT_MAX)
        {
            commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, resource);
        }

        meshInfo.Mesh->Render(commandList);
    }
}

void CustomShaderPass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("Forward G-Buffer");
    for (auto& renderTarget : renderTargetGroup)
    {
        renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}