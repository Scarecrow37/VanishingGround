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

    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

    for (UINT i = 0; i < DXRGBuffer::DXRGBUFFER_END; i++)
    {
        gBufferGroup[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        _gBufferHandles[i] = gBufferGroup[i]->GetRTVHandle();
    }

    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAndPSO();
}

void DXRGBufferPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

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
    commandList->SetGraphicsRootSignature(_fxStaticMesh.GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("cameraData"), cameraData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxStaticMesh.GetRootParameterIndex("matrices"), commandList);

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
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalMesh.GetRootParameterIndex("matrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _fxSkeletalMesh.GetRootParameterIndex("boneMatrices"), commandList);

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
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

    for (auto& gBuffer : gBufferGroup)
    {
        gBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void DXRGBufferPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime)
{
    for (auto& data : _renderDatas)
    {
        data.clear();
    }

    //MeshType meshType = END;
    //for (int i = 0; i < MESH_TYPE_END; i++)
    //{
    //    for (auto& meshInfo : _ownerScene->_activeMeshes[i])
    //    {
    //        const auto& cameraFrustum = _ownerScene->_camera->GetWorldFrustum();

    //        BoundingOrientedBox boundingOrientedBox;
    //        const auto&         meshBoundingBox = meshInfo.Mesh->GetBoundingBox();
    //        meshBoundingBox.Transform(boundingOrientedBox, XMMatrixTranspose(_ownerScene->_matrices[meshInfo.InstanceID].World));

    //        if (!cameraFrustum.Intersects(boundingOrientedBox))
    //        {
    //            continue;
    //        }

    //        // cull_back, cull_front, cull_none
    //        meshType = MeshType(i * 3 + (int)meshInfo.Material.CullMode);
    //        _renderDatas[meshType].emplace_back(meshInfo.Mesh, meshInfo.InstanceID, meshInfo.CustomDepth);
    //    }
    //}
}

void DXRGBufferPass::InitShaderAndPSO()
{
    _psos.resize(MeshType::END);
    PipelineStateStream pss;
    pss.BlendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats        = {{DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
                              DXGI_FORMAT_R32_FLOAT,          // Depth
                              DXGI_FORMAT_R32_UINT},          // CustomDepth
                              DXRGBUFFER_END};
    pss.DSVFormat = _ownerScene->_depthStencilView->GetFormat();

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
            commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_4_objectData"), 3, parameter, 0);
            break;

        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_4_objectData"), 3, parameter, 0);
            break;
        }

        mesh->Render(commandList);
    }
}