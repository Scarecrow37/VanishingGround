#include "pch.h"
#include "BaseMesh.h"
#include "FrameResource.h"
#include "DXRGBufferPass.h"
#include "MeshRenderer.h"
#include "Model.h"

DXRGBufferPass::~DXRGBufferPass() {}

void DXRGBufferPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    static bool isInitialized = false;
    if (!isInitialized)
    {
        auto  mode                = Global::device->GetMode();
        auto& renderTargetManager = Global::multiRenderTargetManager;

        auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, mode.Width, mode.Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        std::initializer_list<std::string_view> renderTargetNames = {"Normal", "Depth", "CustomDepth"};
        auto first = renderTargetNames.begin();

        SharedResource<RenderTarget> renderTarget;
        
        renderTarget = MakeSharedResource<RenderTarget>();
        renderTarget->Initialize(desc, 0.247f);
        renderTargetManager->AddRenderTarget(*(first + DXRGBuffer::DXRNORMAL), renderTarget);

        renderTarget = MakeSharedResource<RenderTarget>();
        desc.Format  = DXGI_FORMAT_R32_FLOAT;
        renderTarget->Initialize(desc, 1.f);
        renderTargetManager->AddRenderTarget(*(first + DXRGBuffer::DXRDEPTH), renderTarget);

        renderTarget = MakeSharedResource<RenderTarget>();
        desc.Format  = DXGI_FORMAT_R32_UINT;
        renderTarget->Initialize(desc, 0.f);
        renderTargetManager->AddRenderTarget(*(first + DXRGBuffer::DXRCUSTOMDEPTH), renderTarget);

        renderTargetManager->AddRenderTargetGroup("GBuffer", renderTargetNames);

        isInitialized = true;
    }

    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    for (UINT i = 0; i < DXRGBuffer::DXRGBUFFER_END; i++)
    {
        gBufferGroup[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        _gBufferHandles[i] = gBufferGroup[i]->GetRTVHandle();
    }

    __super::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
}

void DXRGBufferPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    commandList->OMSetRenderTargets(DXRGBuffer::DXRGBUFFER_END, _gBufferHandles.data(), FALSE, &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &gBufferGroup[0]->GetViewport());
    commandList->RSSetScissorRects(1, &gBufferGroup[0]->GetScissorRect());
}

void DXRGBufferPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    // Static
    commandList->SetGraphicsRootSignature(_staticMeshFX.GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_staticMeshFX.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_staticMeshFX.GetRootParameterIndex("cameraData"), cameraData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _staticMeshFX.GetRootParameterIndex("worldMatrices"),
                                    commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _staticMeshFX.GetRootParameterIndex("material"),
                                    commandList);

    commandList->SetPipelineState(_psos[STATIC_CULL_BACK].Get());
    DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_BACK);

    commandList->SetPipelineState(_psos[STATIC_CULL_FRONT].Get());
    DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_FRONT);

    commandList->SetPipelineState(_psos[STATIC_TWO_SIDED].Get());
    DrawMeshes(commandList, STATIC_MESH, STATIC_TWO_SIDED);

    // Skeletal
    commandList->SetGraphicsRootSignature(_skeletalMeshFX.GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_skeletalMeshFX.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_skeletalMeshFX.GetRootParameterIndex("cameraData"), cameraData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM,
                                    _skeletalMeshFX.GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES,
                                    _skeletalMeshFX.GetRootParameterIndex("boneMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _skeletalMeshFX.GetRootParameterIndex("material"),
                                    commandList);

    commandList->SetPipelineState(_psos[SKELETAL_CULL_BACK].Get());
    DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_CULL_BACK);

    // Skeletal One Sided front
    commandList->SetPipelineState(_psos[SKELETAL_CULL_FRONT].Get());
    DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_CULL_FRONT);

    // Skeletal Two Sided
    commandList->SetPipelineState(_psos[SKELETAL_TWO_SIDED].Get());
    DrawMeshes(commandList, SKELETAL_MESH, SKELETAL_TWO_SIDED);
}

void DXRGBufferPass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    for (auto& gBuffer : gBufferGroup)
    {
        gBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void DXRGBufferPass::Update(ID3D12GraphicsCommandList* commandList) 
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
            const auto&         meshBoundingBox = meshInfo.Mesh->GetBoundingBox();
            meshBoundingBox.Transform(boundingOrientedBox,
                                      XMMatrixTranspose(_ownerScene->_worldMatrices[meshInfo.InstanceID]));

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

void DXRGBufferPass::InitShaderAndPSO()
{
    _psos.resize(MeshType::END);
    PipelineStateStream pss;
    pss.BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats        = { 
        {DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
         DXGI_FORMAT_R32_FLOAT,          // Depth
         DXGI_FORMAT_R32_UINT},
        DXRGBUFFER_END // CustomDepth
    };
    pss.DSVFormat = _ownerScene->_depthStencilView->GetFormat();
    
    _staticMeshFX.SetPipelineStateStream(pss);
    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
    _psos[STATIC_CULL_BACK]          = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_FRONT;
    _psos[STATIC_CULL_FRONT]         = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    _psos[STATIC_TWO_SIDED]          = Global::pipelineStateManager->GetPipelineState(pss);

    _skeletalMeshFX.SetPipelineStateStream(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_BACK;
    _psos[SKELETAL_CULL_BACK]        = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_FRONT;
    _psos[SKELETAL_CULL_FRONT]       = Global::pipelineStateManager->GetPipelineState(pss);

    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    _psos[SKELETAL_TWO_SIDED]        = Global::pipelineStateManager->GetPipelineState(pss);
}

void DXRGBufferPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType)
{
    UINT parameter[3]{0, MAX_BONE_MATRIX, 0};
    for (auto& [mesh, instanceID, customDepth] : _renderDatas[meshType])
    {
        parameter[0] = instanceID;
        parameter[2] = customDepth;

        switch (shaderType)
        {
        case STATIC_MESH:
            commandList->SetGraphicsRoot32BitConstants(_staticMeshFX.GetRootParameterIndex("bit32_3_objectData"), 3,
                                                       parameter, 0);
            break;
        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_skeletalMeshFX.GetRootParameterIndex("bit32_3_objectData"), 3,
                                                       parameter, 0);
            break;
        }

        mesh->Render(commandList);
    }
}