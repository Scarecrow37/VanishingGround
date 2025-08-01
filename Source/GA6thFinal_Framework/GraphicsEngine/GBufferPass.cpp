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
    commandList->SetGraphicsRootSignature(_shaders[STATIC_MESH]->GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_shaders[STATIC_MESH]->GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_shaders[STATIC_MESH]->GetRootParameterIndex("cameraData"), cameraData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _shaders[STATIC_MESH]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _shaders[STATIC_MESH]->GetRootParameterIndex("material"), commandList);

    commandList->SetPipelineState(_psos[STATIC_CULL_BACK].Get());
    DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_BACK);

    commandList->SetPipelineState(_psos[STATIC_CULL_FRONT].Get());    
    DrawMeshes(commandList, STATIC_MESH, STATIC_CULL_FRONT);

    commandList->SetPipelineState(_psos[STATIC_TWO_SIDED].Get());    
    DrawMeshes(commandList, STATIC_MESH, STATIC_TWO_SIDED);

    // Skeletal
    commandList->SetGraphicsRootSignature(_shaders[SKELETAL_MESH]->GetRootSignature());

    commandList->SetGraphicsRootDescriptorTable(_shaders[SKELETAL_MESH]->GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_shaders[SKELETAL_MESH]->GetRootParameterIndex("cameraData"), cameraData);
    frameResource->SetFrameResource(FrameResourceType::TRANSFORM, _shaders[SKELETAL_MESH]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES, _shaders[SKELETAL_MESH]->GetRootParameterIndex("boneMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _shaders[SKELETAL_MESH]->GetRootParameterIndex("material"), commandList);

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

    for (auto& gBuffer : gBufferGroup)
    {
        gBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void GBufferPass::InitShaderAndPSO()
{
    _shaders.reserve(2);
    _psos.resize(MeshType::END);

    std::unique_ptr<ShaderBuilder> staticMeshShaderBuilder = std::make_unique<ShaderBuilder>();
    staticMeshShaderBuilder->BeginBuild();
    staticMeshShaderBuilder->SetShader(L"../Shaders/vs_fr.hlsl", ShaderBuilder::Type::VS);
    staticMeshShaderBuilder->SetShader(L"../Shaders/ps_gbuffer.hlsl", ShaderBuilder::Type::PS);
    staticMeshShaderBuilder->EndBuild();
    _shaders.push_back(std::move(staticMeshShaderBuilder));

    std::unique_ptr<ShaderBuilder> skeletalMeshShaderBuilder = std::make_unique<ShaderBuilder>();
    skeletalMeshShaderBuilder->BeginBuild();
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/vs_skeletal_fr.hlsl", ShaderBuilder::Type::VS);
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/ps_gbuffer.hlsl", ShaderBuilder::Type::PS);
    skeletalMeshShaderBuilder->EndBuild();
    _shaders.push_back(std::move(skeletalMeshShaderBuilder));

    ID3D12Device*                      device = Global::device->GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        pipelineState;

    psodesc.RasterizerState                  = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.BlendState                       = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask                       = UINT_MAX;
    psodesc.PrimitiveTopologyType            = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                      = _shaders[STATIC_MESH]->GetInputLayout();
    psodesc.NumRenderTargets                 = GBuffer::GBUFFER_END;
    psodesc.RTVFormats[GBuffer::BASECOLOR]   = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::NORMAL]      = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::ORM]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::EMISSIVE]    = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::DEPTH]       = DXGI_FORMAT_R32_FLOAT;
    psodesc.RTVFormats[GBuffer::CUSTOMDEPTH] = DXGI_FORMAT_R32_UINT;
    psodesc.DSVFormat                        = _ownerScene->_depthStencilView->GetFormat();
    psodesc.pRootSignature                   = _shaders[STATIC_MESH]->GetRootSignature();
    psodesc.SampleDesc                       = {1, 0};
    psodesc.VS                               = _shaders[STATIC_MESH]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                               = _shaders[STATIC_MESH]->GetShaderByteCode(ShaderBuilder::Type::PS);

    // static one side back.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[STATIC_CULL_BACK] = pipelineState;

    // static one side front.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[STATIC_CULL_FRONT] = pipelineState;

    // static two side.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[STATIC_TWO_SIDED] = pipelineState;

    // Skeletal Mesh PSO
    psodesc.InputLayout    = _shaders[SKELETAL_MESH]->GetInputLayout();
    psodesc.pRootSignature = _shaders[SKELETAL_MESH]->GetRootSignature();
    psodesc.VS             = _shaders[SKELETAL_MESH]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS             = _shaders[SKELETAL_MESH]->GetShaderByteCode(ShaderBuilder::Type::PS);

    // skeletal one side back.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[SKELETAL_CULL_BACK] = pipelineState;

    // skeletal one side front.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[SKELETAL_CULL_FRONT] = pipelineState;

    // skeletal two side.
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos[SKELETAL_TWO_SIDED] = pipelineState;
}

void GBufferPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType)
{
    UINT parameter[3]{0, MAX_BONE_MATRIX, 0};
    for (auto& [mesh, instanceID, customDepth] : _renderDatas[meshType])
    {
        parameter[0] = instanceID;
        parameter[2] = customDepth;

        commandList->SetGraphicsRoot32BitConstants(_shaders[shaderType]->GetRootParameterIndex("bit32_3_objectData"), 3, parameter, 0);
        mesh->Render(commandList);
    }
}