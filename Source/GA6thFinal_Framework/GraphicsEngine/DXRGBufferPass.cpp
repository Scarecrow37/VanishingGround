#include "pch.h"
#include "BaseMesh.h"
#include "FrameResource.h"
#include "DXRGBufferPass.h"
#include "MeshRenderer.h"
#include "Model.h"

DXRGBufferPass::~DXRGBufferPass() {}

void DXRGBufferPass::Initialize(RenderScene* ownerScene, ID3D12GraphicsCommandList* commandList)
{
    static bool isInitialized = false;
    if (!isInitialized)
    {
        auto  mode                = Global::device->GetMode();
        auto& renderTargetManager = Global::multiRenderTargetManager;

        auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, mode.Width, mode.Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

        std::initializer_list<std::string_view> renderTargetNames = {"Normal", "WorldPosition", "Depth", "CustomDepth"};
        auto first = renderTargetNames.begin();

        SharedResource<RenderTarget> renderTarget;
        for (UINT i = 0; i <= DXRGBuffer::DXRWORLDPOSITION; ++i)
        {            
            renderTarget = MakeSharedResource<RenderTarget>();
            renderTarget->Initialize(desc, 0.247f);
            renderTargetManager->AddRenderTarget(*(first + i), renderTarget);
        }

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

    __super::Initialize(ownerScene, commandList);
    InitShaderAndPSO();
}

void DXRGBufferPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    commandList->OMSetRenderTargets(DXRGBuffer::DXRGBUFFER_END, _gBufferHandles.data(), FALSE, &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &gBufferGroup[0]->GetViewPort());
    commandList->RSSetScissorRects(1, &gBufferGroup[0]->GetScissorRect());
}

void DXRGBufferPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    for (auto& data : _renderDatas)
    {
        data.clear();
    }

    UINT instanceID = 0;
    for (auto& [isDestroy, component] : _ownerScene->_meshRenderQueue)
    {
        if (!component->IsActive())
            continue;

        const auto& model = component->GetModel();
        if (!model)
            continue;

        const auto type     = component->GetType();
        MeshType   meshType = MeshType::END;

        const auto& meshes      = model->GetMeshes();
        const auto& materials   = model->GetMaterials();
        UINT        customDepth = component->GetCustomDepth();
        size_t      size        = meshes.size();

        for (size_t i = 0; i < size; i++)
        {
            const auto& material = materials[i];

            switch (type)
            {
            case MeshRenderType::STATIC:
                if (material.IsTwoSided)
                    meshType = STATIC_TWO_SIDED;
                else
                    meshType = STATIC_ONE_SIDED;
                break;

            case MeshRenderType::SKELETAL:
                if (material.IsTwoSided)
                    meshType = SKELETAL_TWO_SIDED;
                else
                    meshType = SKELETAL_ONE_SIDED;
                break;
            }
            _renderDatas[meshType].emplace_back(meshes[i].get(), instanceID++, customDepth);
        }
    }

    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    // Static One Sided
    commandList->SetPipelineState(_psos[STATIC_ONE_SIDED].Get());
    commandList->SetGraphicsRootSignature(_shaders[STATIC]->GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_shaders[STATIC]->GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_shaders[STATIC]->GetRootParameterIndex("cameraData"), cameraData);

    frameResource->SetFrameResource(FrameResourceType::TRANSFORM,
                                    _shaders[STATIC]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _shaders[STATIC]->GetRootParameterIndex("material"),
                                    commandList);
    DrawMeshes(commandList, STATIC, STATIC_ONE_SIDED);

    // Static Two Sided
    commandList->SetPipelineState(_psos[STATIC_TWO_SIDED].Get());
    commandList->SetGraphicsRootSignature(_shaders[STATIC]->GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_shaders[STATIC]->GetRootParameterIndex("textures"), resource);
    commandList->SetGraphicsRootConstantBufferView(_shaders[STATIC]->GetRootParameterIndex("cameraData"), cameraData);

    frameResource->SetFrameResource(FrameResourceType::TRANSFORM,
                                    _shaders[STATIC]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _shaders[STATIC]->GetRootParameterIndex("material"),
                                    commandList);
    DrawMeshes(commandList, STATIC, STATIC_TWO_SIDED);

    // Skeletal One Sided
    commandList->SetPipelineState(_psos[SKELETAL_ONE_SIDED].Get());
    commandList->SetGraphicsRootSignature(_shaders[SKELETAL]->GetRootSignature());
    commandList->SetGraphicsRootConstantBufferView(_shaders[SKELETAL]->GetRootParameterIndex("cameraData"), cameraData);

    frameResource->SetFrameResource(FrameResourceType::TRANSFORM,
                                    _shaders[SKELETAL]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES,
                                    _shaders[SKELETAL]->GetRootParameterIndex("boneMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _shaders[SKELETAL]->GetRootParameterIndex("material"),
                                    commandList);
    DrawMeshes(commandList, SKELETAL, SKELETAL_ONE_SIDED);

    // Skeletal Two Sided
    commandList->SetPipelineState(_psos[SKELETAL_TWO_SIDED].Get());
    commandList->SetGraphicsRootSignature(_shaders[SKELETAL]->GetRootSignature());
    commandList->SetGraphicsRootConstantBufferView(_shaders[SKELETAL]->GetRootParameterIndex("cameraData"), cameraData);

    frameResource->SetFrameResource(FrameResourceType::TRANSFORM,
                                    _shaders[SKELETAL]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::BONE_MATRICES,
                                    _shaders[SKELETAL]->GetRootParameterIndex("boneMatrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::MATERIAL, _shaders[SKELETAL]->GetRootParameterIndex("material"),
                                    commandList);
    DrawMeshes(commandList, SKELETAL, SKELETAL_TWO_SIDED);
}

void DXRGBufferPass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");

    for (auto& gBuffer : gBufferGroup)
    {
        gBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void DXRGBufferPass::InitShaderAndPSO()
{
    _shaders.reserve(2);
    _psos.reserve(4);

    std::unique_ptr<ShaderBuilder> staticMeshShaderBuilder = std::make_unique<ShaderBuilder>();
    staticMeshShaderBuilder->BeginBuild();
    staticMeshShaderBuilder->SetShader(L"../Shaders/vs_fr.hlsl", ShaderBuilder::Type::VS);
    staticMeshShaderBuilder->SetShader(L"../Shaders/ps_dxrgbuffer.hlsl", ShaderBuilder::Type::PS);
    staticMeshShaderBuilder->EndBuild();
    _shaders.push_back(std::move(staticMeshShaderBuilder));

    std::unique_ptr<ShaderBuilder> skeletalMeshShaderBuilder = std::make_unique<ShaderBuilder>();
    skeletalMeshShaderBuilder->BeginBuild();
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/vs_skeletal_fr.hlsl", ShaderBuilder::Type::VS);
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/ps_dxrgbuffer.hlsl", ShaderBuilder::Type::PS);
    skeletalMeshShaderBuilder->EndBuild();
    _shaders.push_back(std::move(skeletalMeshShaderBuilder));

    ID3D12Device*                      device = Global::device->GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        staticTwoSidedPSO;

    psodesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode           = D3D12_CULL_MODE_NONE;
    psodesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask                         = UINT_MAX;
    psodesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                        = _shaders[STATIC]->GetInputLayout();
    psodesc.NumRenderTargets                         = DXRGBuffer::DXRGBUFFER_END;
    psodesc.RTVFormats[DXRGBuffer::DXRNORMAL]        = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[DXRGBuffer::DXRWORLDPOSITION] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[DXRGBuffer::DXRDEPTH]         = DXGI_FORMAT_R32_FLOAT;
    psodesc.RTVFormats[DXRGBuffer::DXRCUSTOMDEPTH]   = DXGI_FORMAT_R32_UINT;
    psodesc.DSVFormat                          = _ownerScene->_depthStencilView->GetFormat();
    psodesc.pRootSignature                     = _shaders[STATIC]->GetRootSignature();
    psodesc.SampleDesc                         = {1, 0};
    psodesc.VS                                 = _shaders[STATIC]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                                 = _shaders[STATIC]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&staticTwoSidedPSO));
    FAILED_CHECK_MESSAGE(hr, L"DXRGBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(staticTwoSidedPSO);

    // static one side.
    ComPtr<ID3D12PipelineState> staticOneSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&staticOneSidePSO));
    FAILED_CHECK_MESSAGE(hr, L"DXRGBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(staticOneSidePSO);

    // skeletal two side.
    ComPtr<ID3D12PipelineState> skeletalTwoSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psodesc.InputLayout              = _shaders[SKELETAL]->GetInputLayout();
    psodesc.pRootSignature           = _shaders[SKELETAL]->GetRootSignature();
    psodesc.VS                       = _shaders[SKELETAL]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                       = _shaders[SKELETAL]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&skeletalTwoSidePSO));
    FAILED_CHECK_MESSAGE(hr, L"DXRGBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(skeletalTwoSidePSO);

    // skeletal one side.
    ComPtr<ID3D12PipelineState> skeletalOneSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&skeletalOneSidePSO));
    FAILED_CHECK_MESSAGE(hr, L"DXRGBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(skeletalOneSidePSO);
}

void DXRGBufferPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, int shaderType, MeshType meshType)
{
    UINT parameter[3]{0, MAX_BONE_MATRIX, 0};
    for (auto& [mesh, instanceID, customDepth] : _renderDatas[meshType])
    {
        parameter[0] = instanceID;
        parameter[2] = customDepth;

        commandList->SetGraphicsRoot32BitConstants(_shaders[shaderType]->GetRootParameterIndex("bit32_3_objectData"), 3,
                                                   parameter, 0);
        mesh->Render(commandList);
    }
}