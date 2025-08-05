#include "pch.h"
#include "GBufferPass.h"
#include "BaseMesh.h"
#include "FrameResource.h"
#include "MeshRenderer.h"
#include "Model.h"

GBufferPass::~GBufferPass() {}

void GBufferPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    static bool isInitialized = false;
    if (!isInitialized)
    {
        auto  mode                = Global::device->GetMode();
        auto& renderTargetManager = Global::multiRenderTargetManager;

        auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, mode.Width, mode.Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        std::initializer_list<std::string_view> renderTargetNames = {
            "BaseColor", "Normal", "ORM", "Emissive", "Depth", "CustomDepth"};
        auto first = renderTargetNames.begin();

        SharedResource<RenderTarget> renderTarget;
        for (UINT i = 0; i <= GBuffer::EMISSIVE; ++i)
        {
            renderTarget = MakeSharedResource<RenderTarget>();
            renderTarget->Initialize(desc, 0.247f);
            renderTargetManager->AddRenderTarget(*(first + i), renderTarget);
        }

        renderTarget = MakeSharedResource<RenderTarget>();
        desc.Format  = DXGI_FORMAT_R32_FLOAT;
        renderTarget->Initialize(desc, 1.f);
        renderTargetManager->AddRenderTarget(*(first + GBuffer::DEPTH), renderTarget);

        renderTarget = MakeSharedResource<RenderTarget>();
        desc.Format  = DXGI_FORMAT_R32_UINT;
        renderTarget->Initialize(desc, 0.f);
        renderTargetManager->AddRenderTarget(*(first + GBuffer::CUSTOMDEPTH), renderTarget);

        renderTargetManager->AddRenderTargetGroup("GBuffer", renderTargetNames);        

        isInitialized = true;
    }

    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    for (UINT i = 0; i < GBuffer::GBUFFER_END; i++)
    {
        gBufferGroup[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        _gBufferHandles[i] = gBufferGroup[i]->GetRTVHandle();
    }

    __super::Initialize(ownerScene, ownerTechnique, commandList);

    InitShaderAndPSO();
}

void GBufferPass::AddRenderPassDatas(std::string_view sceneName)
{
    auto desc = Global::multiRenderTargetManager->GetRenderTarget("BaseColor")->GetResource()->GetDesc();

    for (auto& renderTarget : _gBufferRenderTargets)
    {
        renderTarget = MakeSharedResource<RenderTarget>();
        renderTarget->Initialize(desc, 0.247f);
    }

    Global::renderPassDatas->AddRenderPassImage(sceneName, "G-BufferPass", "Albedo", _gBufferRenderTargets[0]->GetSRVHandle());
    Global::renderPassDatas->AddRenderPassImage(sceneName, "G-BufferPass", "Normal", _gBufferRenderTargets[1]->GetSRVHandle());
    Global::renderPassDatas->AddRenderPassImage(sceneName, "G-BufferPass", "ORM", _gBufferRenderTargets[2]->GetSRVHandle());
    Global::renderPassDatas->AddRenderPassImage(sceneName, "G-BufferPass", "Emissive", _gBufferRenderTargets[3]->GetSRVHandle());
}

void GBufferPass::Update(ID3D12GraphicsCommandList* commadList)
{    
    for (auto& data : _renderDatas)
    {
        data.clear();
    }

    MeshType meshType = END;
    for (int i = 0; i < MESH_TYPE_END; i++)
    {
        for (auto& meshInfo : _ownerScene->_activeMeshes[i])
        {
            const auto& cameraFrustum = _ownerScene->_camera->GetWorldFrustum();
            
            BoundingOrientedBox boundingOrientedBox;
            const auto& meshBoundingBox = meshInfo.Mesh->GetBoundingBox();
            meshBoundingBox.Transform(boundingOrientedBox, XMMatrixTranspose(_ownerScene->_worldMatrices[meshInfo.InstanceID]));

            if (!cameraFrustum.Intersects(boundingOrientedBox))
            {
                continue;
            }

            // cull_back, cull_front, cull_none
            meshType = MeshType(i * 3 + (int)meshInfo.Material.CullMode);
            _renderDatas[meshType].emplace_back(meshInfo.Mesh, meshInfo.InstanceID, meshInfo.CustomDepth);
        }
    }
}

void GBufferPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    commandList->OMSetRenderTargets(GBuffer::GBUFFER_END, _gBufferHandles.data(), FALSE, &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &gBufferGroup[0]->GetViewport());
    commandList->RSSetScissorRects(1, &gBufferGroup[0]->GetScissorRect());
}

void GBufferPass::Draw(ID3D12GraphicsCommandList* commandList)
{    
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    // Static
    commandList->SetGraphicsRootSignature(_fxStaticMesh.GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("cameraData"), cameraData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxStaticMesh.GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _fxStaticMesh.GetRootParameterIndex("material"), commandList);

    commandList->SetPipelineState(_psos[STATIC_CULL_BACK].Get());
    DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_BACK);

    commandList->SetPipelineState(_psos[STATIC_CULL_FRONT].Get());    
    DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_FRONT);

    commandList->SetPipelineState(_psos[STATIC_TWO_SIDED].Get());    
    DrawMeshes(commandList, STATIC_MESH, STATIC_TWO_SIDED);

    // Skeletal
    commandList->SetGraphicsRootSignature(_fxSkeletalMesh.GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("cameraData"), cameraData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalMesh.GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _fxSkeletalMesh.GetRootParameterIndex("boneMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _fxSkeletalMesh.GetRootParameterIndex("material"), commandList);

    commandList->SetPipelineState(_psos[SKELETAL_CULL_BACK].Get());
    DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_CULL_BACK);

    // Skeletal One Sided front
    commandList->SetPipelineState(_psos[SKELETAL_CULL_FRONT].Get());
    DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_CULL_FRONT);

    // Skeletal Two Sided
    commandList->SetPipelineState(_psos[SKELETAL_TWO_SIDED].Get());
    DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_TWO_SIDED);
}

void GBufferPass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    for (int i = 0; i < 4; i++)
    {
        _gBufferRenderTargets[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
        gBufferGroup[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);

        commandList->CopyResource(_gBufferRenderTargets[i]->GetResource(), gBufferGroup[i]->GetResource());

        _gBufferRenderTargets[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    for (auto& gBuffer : gBufferGroup)
    {
        gBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void GBufferPass::InitShaderAndPSO()
{
    _psos.resize(MeshType::END);

    PipelineStateStream pss;
    pss.BlendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats        = {{DXGI_FORMAT_R32G32B32A32_FLOAT, // BaseColor
                              DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
                              DXGI_FORMAT_R32G32B32A32_FLOAT, // ORM
                              DXGI_FORMAT_R32G32B32A32_FLOAT, // Emissive
                              DXGI_FORMAT_R32_FLOAT,          // Depth
                              DXGI_FORMAT_R32_UINT},          // CustomDepth
                             GBUFFER_END};
    pss.DSVFormat         = _ownerScene->_depthStencilView->GetFormat();

    _fxStaticMesh.SetPipelineStateStream(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
    _psos[STATIC_CULL_BACK]          = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_FRONT;
    _psos[STATIC_CULL_FRONT]         = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    _psos[STATIC_TWO_SIDED]          = Global::pipelineStateManager->GetPipelineState(pss);

    _fxSkeletalMesh.SetPipelineStateStream(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
    _psos[SKELETAL_CULL_BACK]        = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_FRONT;
    _psos[SKELETAL_CULL_FRONT]       = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    _psos[SKELETAL_TWO_SIDED]        = Global::pipelineStateManager->GetPipelineState(pss);
}

void GBufferPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType)
{
    UINT parameter[3]{0, MAX_BONE_MATRIX, 0};
    for (auto& [mesh, instanceID, customDepth] : _renderDatas[meshType])
    {
        parameter[0] = instanceID;
        parameter[2] = customDepth;

        switch (shaderType)
        {
        case STATIC_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_3_objectData"), 3, parameter, 0);
            break;
        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_3_objectData"), 3, parameter, 0);
            break;
        }

        mesh->Render(commandList);
    }
}