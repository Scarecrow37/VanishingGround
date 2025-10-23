#include "pch.h"
#include "ForwardPBRLitPass.h"
#include "FrameResource.h"
#include "ShadowMapPass.h"
#include "SkyBox.h"

ForwardPBRLitPass::~ForwardPBRLitPass() = default;

void ForwardPBRLitPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    D3D12_RENDER_TARGET_BLEND_DESC rtDesc{};
    rtDesc.SrcBlend              = D3D12_BLEND_ONE;
    rtDesc.DestBlend             = D3D12_BLEND_ZERO;
    rtDesc.BlendOp               = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha         = D3D12_BLEND_ONE;
    rtDesc.DestBlendAlpha        = D3D12_BLEND_ZERO;
    rtDesc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
           
    PipelineStateStream pss;
    pss.BlendState                            = CD3DX12_BLEND_DESC(CommonStates::NonPremultiplied);
    (&pss.BlendState)->AlphaToCoverageEnable  = FALSE;
    (&pss.BlendState)->IndependentBlendEnable = TRUE;
    (&pss.BlendState)->RenderTarget[1]        = rtDesc;
    (&pss.BlendState)->RenderTarget[2]        = rtDesc;
    (&pss.BlendState)->RenderTarget[3]        = rtDesc;
    pss.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                     = CD3DX12_DEPTH_STENCIL_DESC(CommonStates::DepthRead);
    pss.DSVFormat                             = _ownerScene->_depthStencilView->GetFormat();
    pss.PrimitiveTopology                     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                            = {{DXGI_FORMAT_R32G32B32A32_FLOAT, // Result
                                                  DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
                                                  DXGI_FORMAT_R32_FLOAT,          // Depth
                                                  DXGI_FORMAT_R32_UINT},          // CustomDepth
                                                 4};

    auto CreatePipelineStateStream = [this, &pss](int meshType)
    {
        (&pss.RasterizerState)->CullMode      = D3D12_CULL_MODE_BACK;
        _psos[meshType][CULL_BACK] = Global::pipelineStateManager->GetPipelineState(pss);
    
        (&pss.RasterizerState)->CullMode       = D3D12_CULL_MODE_FRONT;
        _psos[meshType][CULL_FRONT] = Global::pipelineStateManager->GetPipelineState(pss);
    
        (&pss.RasterizerState)->CullMode      = D3D12_CULL_MODE_NONE;
        _psos[meshType][TWO_SIDED] = Global::pipelineStateManager->GetPipelineState(pss);
    };
    
    _fxStaticMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(STATIC_MESH);
    
    _fxSkeletalMesh.SetPipelineStateStream(pss);
    CreatePipelineStateStream(SKELETAL_MESH);
}

void ForwardPBRLitPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime)
{
    for (auto& renderData : _renderDatas)
    {
        for (auto& j : renderData)
        {
            j.clear();
        }
    }

    XMVECTOR cameraPosition = _ownerScene->_camera->GetPosition();
    XMVECTOR cameraForward  = -_ownerScene->_camera->GetWorldMatrix().Forward();
    std::vector<std::pair<int, MeshInfo*>> translucentMeshes;
    for (int i = 0; i < MESH_TYPE_END; i++)
    {
        for (auto& activeMeshe : _ownerScene->_activeMeshes[i])
        {
            if (activeMeshe.Material.BlendMode != Material::BlendModeType::TRANSLUCENT || 0.f == activeMeshe.Material.Alpha)
                continue;

            XMVECTOR center      = XMLoadFloat3(&activeMeshe.Mesh->GetBoundingBox().Center);
            XMVECTOR dot         = XMVector3Dot(center - cameraPosition, cameraForward);
            activeMeshe.DepthKey = dot.m128_f32[0];
            translucentMeshes.emplace_back(i, &activeMeshe);
        }
    }

    std::stable_sort(translucentMeshes.begin(), translucentMeshes.end(),
                     [this](const std::pair<int, MeshInfo*>& a, const std::pair<int, MeshInfo*>& b) {
                         return a.second->DepthKey > b.second->DepthKey;
                     });

    for (auto& [meshType, meshInfo] : translucentMeshes)
    {
        const auto& cameraFrustum = _ownerScene->_camera->GetWorldFrustum();

        /*BoundingOrientedBox boundingOrientedBox;
        const auto&         meshBoundingBox = meshInfo->Mesh->GetBoundingBox();
        meshBoundingBox.Transform(boundingOrientedBox, XMMatrixTranspose(_ownerScene->_matrices[meshInfo->InstanceData.MatrixID].World));

        if (!cameraFrustum.Intersects(boundingOrientedBox))
        {
            continue;
        }*/

        int cullMode = (int)meshInfo->Material.CullMode;
        _renderDatas[meshType][cullMode].emplace_back(meshInfo->Mesh, &meshInfo->InstanceData);
    }
}

void ForwardPBRLitPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("Forward G-Buffer");
    for (auto& renderTarget : renderTargetGroup)
    {
        renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }
    
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[4]{};
    rtvHandles[0] = _meshRenderTarget->GetRTVHandle();
    for (int i = 1; i < 4; i++)
    {
        rtvHandles[i] = renderTargetGroup[i - 1]->GetRTVHandle();
    }

    commandList->OMSetRenderTargets(4, rtvHandles, FALSE, &_ownerScene->_depthStencilView->GetDSVHandle());
}

void ForwardPBRLitPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto  lightData              = _ownerScene->_lightBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];
    auto  shadowMapPass          = _ownerTechnique->GetRenderPass<ShadowMapPass>();

    if (nullptr == shadowMapPass)
        return;

    D3D12_GPU_DESCRIPTOR_HANDLE brdf;
    D3D12_GPU_DESCRIPTOR_HANDLE irradiance;
    D3D12_GPU_DESCRIPTOR_HANDLE prefiltered;

    if (_ownerScene->_skyBox->HasIBLTexture())
    {
        brdf        = _ownerScene->_skyBox->GetBrdfLUTSRV();
        irradiance  = _ownerScene->_skyBox->GetIrradianceMapSRV();
        prefiltered = _ownerScene->_skyBox->GetPrefilteredMapSRV();
    }
    else
    {
        auto defaultTexture = Global::resourceManager->LoadResource<Texture>("BlackTexture")->GetGPUHandle();
        brdf                = defaultTexture;
        irradiance          = defaultTexture;
        prefiltered         = defaultTexture;
    }

    // Static
    commandList->SetGraphicsRootSignature(_fxStaticMesh.GetRootSignature());
    commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_4_numLight"), 4, &_ownerScene->_numLight, 0);    
    commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("lightData"), lightData);
    commandList->SetGraphicsRootConstantBufferView(_fxStaticMesh.GetRootParameterIndex("cascadeData"), shadowMapPass->GetCascadeDataCBV());
    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("shadowMap"), shadowMapPass->GetShadowMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("irradianceMap"), irradiance);
    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("prefilteredMap"), prefiltered);
    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("brdfLUT"), brdf);
    commandList->SetGraphicsRootDescriptorTable(_fxStaticMesh.GetRootParameterIndex("textures"), resource);

    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxStaticMesh.GetRootParameterIndex("matrices"), commandList);

    commandList->SetPipelineState(_psos[STATIC_MESH][CULL_BACK].Get());
    DrawMeshes(commandList, STATIC_MESH, CULL_BACK);

    commandList->SetPipelineState(_psos[STATIC_MESH][CULL_FRONT].Get());
    DrawMeshes(commandList, STATIC_MESH, CULL_FRONT);

    commandList->SetPipelineState(_psos[STATIC_MESH][TWO_SIDED].Get());
    DrawMeshes(commandList, STATIC_MESH, TWO_SIDED);

    // Skeletal
    commandList->SetGraphicsRootSignature(_fxSkeletalMesh.GetRootSignature());
    commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_4_numLight"), 4, &_ownerScene->_numLight, 0);
    commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("lightData"), lightData);
    commandList->SetGraphicsRootConstantBufferView(_fxSkeletalMesh.GetRootParameterIndex("cascadeData"), shadowMapPass->GetCascadeDataCBV());
    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("shadowMap"), shadowMapPass->GetShadowMapSRV());
    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("irradianceMap"), irradiance);
    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("prefilteredMap"), prefiltered);
    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("brdfLUT"), brdf);
    commandList->SetGraphicsRootDescriptorTable(_fxSkeletalMesh.GetRootParameterIndex("textures"), resource);

    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _fxSkeletalMesh.GetRootParameterIndex("matrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _fxSkeletalMesh.GetRootParameterIndex("boneMatrices"), commandList);

    commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_BACK].Get());
    DrawMeshes(commandList, SKELETAL_MESH, CULL_BACK);

    commandList->SetPipelineState(_psos[SKELETAL_MESH][CULL_FRONT].Get());
    DrawMeshes(commandList, SKELETAL_MESH, CULL_FRONT);

    commandList->SetPipelineState(_psos[SKELETAL_MESH][TWO_SIDED].Get());
    DrawMeshes(commandList, SKELETAL_MESH, TWO_SIDED);
}

void ForwardPBRLitPass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("Forward G-Buffer");
    for (auto& renderTarget : renderTargetGroup)
    {
        renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }    
}

void ForwardPBRLitPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, MeshType meshType, CullMode cullMode)
{
    const auto& parallaxMappingProperty = std::any_cast<const ParallaxMappingProperty&>(Global::renderPassDatas->GetRenderPassProperty("G-BufferPass"));

    switch (meshType)
    {
    case STATIC_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_2_gbufferData"), 2, &parallaxMappingProperty, 0);
        break;
    case SKELETAL_MESH:
        commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_2_gbufferData"), 2, &parallaxMappingProperty, 0);
        break;
    }

    for (auto& [mesh, instanceData] : _renderDatas[meshType][cullMode])
    {
        switch (meshType)
        {
        case STATIC_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxStaticMesh.GetRootParameterIndex("bit32_7_instanceData"), 7, instanceData, 0);
            break;
        case SKELETAL_MESH:
            commandList->SetGraphicsRoot32BitConstants(_fxSkeletalMesh.GetRootParameterIndex("bit32_7_instanceData"), 7, instanceData, 0);
            break;
        }

        mesh->Render(commandList);
    }
}