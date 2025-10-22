#include "pch.h"
#include "DXRGBufferPass.h"
#include "FrameResource.h"
#include "BaseMesh.h"

void DXRGBufferPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

    for (UINT i = 0; i < DXRGBuffer::DXRGBUFFER_END; ++i)
    {
        gBufferGroup[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        _gBufferHandles[i] = gBufferGroup[i]->GetRTVHandle();
    }
    InitShaderAndPSO();

    _instanceDatasBuffer = std::make_unique<StructuredBuffer>();
    _instanceDatasBuffer->Initialize(sizeof(InstanceData), MAX_OBJECTS);
}

void DXRGBufferPass::AddRenderPassDatas(std::string_view sceneName) 
{
    auto desc = Global::multiRenderTargetManager->GetRenderTarget("BaseColor")->GetResource()->GetDesc();
    for (auto& renderTarget : _gBufferRenderTargets)
    {
        renderTarget = MakeSharedResource<RenderTarget>();
        renderTarget->Initialize(desc, 0.247f);
    }
    Global::renderPassDatas->AddRenderPassImage(sceneName, "G-BufferPass", "Normal", _gBufferRenderTargets[0]->GetSRVHandle());
    Global::renderPassDatas->AddRenderPassImage(sceneName, "G-BufferPass", "Depth", _gBufferRenderTargets[1]->GetSRVHandle());

    Global::renderPassDatas->AddRenderPassProperty("G-BufferPass", ParallaxMappingProperty(2.9f, 0.f));
}

void DXRGBufferPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime) 
{
    for (auto& mesh : _mesheInfos)
    {
        for (auto& j : mesh)
        {
            for (auto& k : j)
            {
                k.clear();
            }
        }
    }

    for (int i = 0; i < MESH_TYPE_END; ++i)
    {
        for (auto& meshInfo : _ownerScene->_activeMeshes[i])
        {
            int      blendMode = (int)meshInfo.Material.BlendMode;
            if (blendMode == Material::BlendModeType::TRANSLUCENT)
                continue;
            
            const auto& cameraFrustum = _ownerScene->_camera->GetWorldFrustum();

            BoundingOrientedBox boundingOrientedBox;
            const auto&         meshBoundingBox = meshInfo.Mesh->GetBoundingBox();
            meshBoundingBox.Transform(boundingOrientedBox,
                                      XMMatrixTranspose(_ownerScene->_matrices[meshInfo.InstanceData.MatrixID].World));

            if (!cameraFrustum.Intersects(boundingOrientedBox))
            {
                continue;
            }
            
            int cullMode = (int)meshInfo.Material.CullMode;
            _mesheInfos[i][blendMode][cullMode].push_back(&meshInfo);
        }
    }
    
    _instanceDatas.clear();

    for (int i = 0; i < Material::BlendModeType::BMT_END - 1; ++i)
    {
        for (int j = 0; j < CullMode::END; ++j)
        {
            for (auto& meshInfo : _mesheInfos[STATIC_MESH][i][j])
            {
                _instanceDatas.emplace_back(meshInfo->InstanceData);
            }
        }
    }
    for (int i = 0; i < Material::BlendModeType::BMT_END - 1; ++i)
    {
        for (int j = 0; j < CullMode::END; ++j)
        {
            for (auto& meshInfo : _mesheInfos[SKELETAL_MESH][i][j])
            {
                _instanceDatas.emplace_back(meshInfo->InstanceData);
            }
        }
    }
    _instanceDatasBuffer->CopyStructuredBuffer(commandList, _instanceDatas.data(), (UINT)_instanceDatas.size());
}

void DXRGBufferPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

    commandList->OMSetRenderTargets(DXRGBuffer::DXRGBUFFER_END, _gBufferHandles.data(), FALSE,
                                    &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &gBufferGroup[0]->GetViewport());
    commandList->RSSetScissorRects(1, &gBufferGroup[0]->GetScissorRect());
}

void DXRGBufferPass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto  instanceData           = _instanceDatasBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    UINT offset = 0;

    // --- Static Meshes ---
    commandList->SetGraphicsRootSignature(_fxStaticMesh.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetGraphicsRootShaderResourceView(_fxStaticMesh.GetRootParameterIndex("instanceData"), instanceData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxStaticMesh.GetRootParameterIndex("matrices"),
                                    commandList);

    for (int i = 0; i < Material::BlendModeType::BMT_END - 1; i++)
    {
        commandList->SetPipelineState(_psos[STATIC_MESH][i][CULL_BACK].Get());
        DrawMeshes(commandList, STATIC_MESH, (Material::BlendModeType)i, CULL_BACK, offset);
        offset += (UINT)_mesheInfos[STATIC_MESH][i][CULL_BACK].size();

        commandList->SetPipelineState(_psos[STATIC_MESH][i][CULL_FRONT].Get());
        DrawMeshes(commandList, STATIC_MESH, (Material::BlendModeType)i, CULL_FRONT, offset);
        offset += (UINT)_mesheInfos[STATIC_MESH][i][CULL_FRONT].size();

        commandList->SetPipelineState(_psos[STATIC_MESH][i][TWO_SIDED].Get());
        DrawMeshes(commandList, STATIC_MESH, (Material::BlendModeType)i, TWO_SIDED, offset);
        offset += (UINT)_mesheInfos[STATIC_MESH][i][TWO_SIDED].size();
    }

    // --- Skeletal Meshes ---
    commandList->SetGraphicsRootSignature(_fxSkeletalMesh.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetGraphicsRootShaderResourceView(_fxSkeletalMesh.GetRootParameterIndex("instanceData"), instanceData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalMesh.GetRootParameterIndex("matrices"),
                                    commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES,
                                    _fxSkeletalMesh.GetRootParameterIndex("boneMatrices"), commandList);

    for (int i = 0; i < Material::BlendModeType::BMT_END - 1; i++)
    {
        commandList->SetPipelineState(_psos[SKELETAL_MESH][i][CULL_BACK].Get());
        DrawMeshes(commandList, SKELETAL_MESH, (Material::BlendModeType)i, CULL_BACK, offset);
        offset += (UINT)_mesheInfos[SKELETAL_MESH][i][CULL_BACK].size();

        commandList->SetPipelineState(_psos[SKELETAL_MESH][i][CULL_FRONT].Get());
        DrawMeshes(commandList, SKELETAL_MESH, (Material::BlendModeType)i, CULL_FRONT, offset);
        offset += (UINT)_mesheInfos[SKELETAL_MESH][i][CULL_FRONT].size();

        commandList->SetPipelineState(_psos[SKELETAL_MESH][i][TWO_SIDED].Get());
        DrawMeshes(commandList, SKELETAL_MESH, (Material::BlendModeType)i, TWO_SIDED, offset);
        offset += (UINT)_mesheInfos[SKELETAL_MESH][i][TWO_SIDED].size();
    }
}

void DXRGBufferPass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

    for (int i = 0; i < 3; i++)
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


void DXRGBufferPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, Material::BlendModeType blendModeType, CullMode cullMode, UINT offset)
{
    const auto& parallaxMappingProperty =
        std::any_cast<const ParallaxMappingProperty&>(Global::renderPassDatas->GetRenderPassProperty("G-BufferPass"));

    switch (meshType)
    {
    case STATIC_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_2_gbufferData"), 2,
                                                   &parallaxMappingProperty, 0);
        break;
    case SKELETAL_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_2_gbufferData"), 2,
                                                   &parallaxMappingProperty, 0);
        break;
    }

    UINT      instanceCount = 0;
    BaseMesh* previousMesh  = nullptr;
    BaseMesh* currentMesh   = nullptr;
    for (auto& meshInfo : _mesheInfos[meshType][blendModeType][cullMode])
    {
        if (nullptr == previousMesh)
        {
            currentMesh   = meshInfo->Mesh;
            previousMesh  = meshInfo->Mesh;
            instanceCount = 1;
            continue;
        }

        if (meshInfo->Mesh != previousMesh)
        {
            switch (meshType)
            {
            case STATIC_MESH:
                commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_1_offset"), 1,
                                                           &offset, 0);
                break;
            case SKELETAL_MESH:
                commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_1_offset"), 1,
                                                           &offset, 0);
                break;
            }

            previousMesh->Render(commandList, instanceCount);
            previousMesh = meshInfo->Mesh;
            offset += instanceCount;
            instanceCount = 1;
        }
        else
        {
            instanceCount++;
        }

        currentMesh = meshInfo->Mesh;
    }

    if (nullptr != currentMesh)
    {
        switch (meshType)
        {
        case STATIC_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_1_offset"), 1,
                                                       &offset, 0);
            break;
        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_1_offset"), 1,
                                                       &offset, 0);
            break;
        }

        currentMesh->Render(commandList, instanceCount);
    }
}

void DXRGBufferPass::InitShaderAndPSO()
{
    PipelineStateStream pss;
    pss.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats        = {{DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
                              DXGI_FORMAT_R32_FLOAT,          // Depth
                              DXGI_FORMAT_R32_UINT},          // CustomDepth
                             DXRGBUFFER_END};
    pss.DSVFormat         = _ownerScene->_depthStencilView->GetFormat();

    auto CreatePipelineStateStream = [this, &pss](int meshType, int blendMode)
    {
        (&pss.RasterizerState)->CullMode       = D3D12_CULL_MODE_BACK;
        _psos[meshType][blendMode][CULL_BACK]  = Global::pipelineStateManager->GetPipelineState(pss);
        (&pss.RasterizerState)->CullMode       = D3D12_CULL_MODE_FRONT;
        _psos[meshType][blendMode][CULL_FRONT] = Global::pipelineStateManager->GetPipelineState(pss);
        (&pss.RasterizerState)->CullMode       = D3D12_CULL_MODE_NONE;
        _psos[meshType][blendMode][TWO_SIDED]  = Global::pipelineStateManager->GetPipelineState(pss);
    };
    _fxStaticMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(STATIC_MESH, (int)Material::BlendModeType::OPAQUE);
    _fxSkeletalMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(SKELETAL_MESH, (int)Material::BlendModeType::OPAQUE);

    FX<GE::VS::STATIC_FR, GE::PS::DXRGBUFFER_MASKED> fxStaticMeshMasked;
    fxStaticMeshMasked.SetPipelineStateStream(pss);
    CreatePipelineStateStream(STATIC_MESH, (int)Material::BlendModeType::MASKED);

    FX<GE::VS::SKELETAL_FR, GE::PS::DXRGBUFFER_MASKED> fxSkeletalMeshMasked;
    fxSkeletalMeshMasked.SetPipelineStateStream(pss);
    CreatePipelineStateStream(SKELETAL_MESH, (int)Material::BlendModeType::MASKED);
}