#include "pch.h"
#include "GBufferPass.h"
#include "BaseMesh.h"
#include "FrameResource.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "RenderScene.h"
#include "RenderTarget.h"

GBufferPass::~GBufferPass() {}

void GBufferPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    static bool isInitialized = false;
    if (!isInitialized)
    {
        auto  mode                = UmDevice.GetMode();
        auto& renderTargetManager = UmMultiRenderTargetManager;

        std::initializer_list<std::string_view> renderTargetNames = {
            "BaseColor", "Normal", "ORM", "Emissive", "WorldPosition", "Depth", "CustomDepth"};
        auto first = renderTargetNames.begin();

        std::unique_ptr<RenderTarget> renderTarget;
        for (UINT i = 0; i <= GBuffer::WORLDPOSITION; ++i)
        {
            renderTarget = std::make_unique<RenderTarget>();
            renderTarget->Initialize(mode.Width, mode.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, 0.247f);
            renderTarget->CreateShaderResourceView();
            renderTargetManager.AddRenderTarget(*(first + i), std::move(renderTarget));
        }

        renderTarget = std::make_unique<RenderTarget>();
        renderTarget->Initialize(mode.Width, mode.Height, DXGI_FORMAT_R32_FLOAT, 1.f);
        renderTarget->CreateShaderResourceView();
        renderTargetManager.AddRenderTarget(*(first + GBuffer::DEPTH), std::move(renderTarget));

        renderTarget = std::make_unique<RenderTarget>();
        renderTarget->Initialize(mode.Width, mode.Height, DXGI_FORMAT_R32_UINT, 0.f);
        renderTarget->CreateShaderResourceView();
        renderTargetManager.AddRenderTarget(*(first + GBuffer::CUSTOMDEPTH), std::move(renderTarget));

        renderTargetManager.AddRenderTargetGroup("GBuffer", renderTargetNames);

        const auto&                gBufferGroup = renderTargetManager.GetRenderTargetGroup("GBuffer");
        ID3D12GraphicsCommandList* commandList = UmDevice.GetCommandList();

        for (UINT i = 0; i < GBuffer::GBUFFER_END; i++)
        {
            gBufferGroup[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            _gBufferHandles[i] = gBufferGroup[i]->GetRTVHandle();
        }

        isInitialized = true;
    }

    __super::Initialize(viewPort, sissorRect);
    InitShaderAndPSO();
}

void GBufferPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    // GBuffer -> RENDER_TARGET 전이 + Clear
    const auto& gBufferGroup = UmMultiRenderTargetManager.GetRenderTargetGroup("GBuffer");

    for (UINT i = 0; i < GBuffer::GBUFFER_END; i++)
    {
        gBufferGroup[i]->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
        gBufferGroup[i]->ClearRenderTarget(commandList);
        _gBufferHandles[i] = gBufferGroup[i]->GetRTVHandle();
    }

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &br);
    commandList->ClearDepthStencilView(_ownerScene->_depthStencilHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(GBuffer::GBUFFER_END, _gBufferHandles.data(), FALSE, &_ownerScene->_depthStencilHandle);
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void GBufferPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    std::vector<MeshRenderer*> meshes[MeshType::END];

    for (auto& [isDestroy, component] : _ownerScene->_renderQueue)
    {
        if (!component->IsActive())
            continue;

        const auto type = component->GetType();

        switch (type)
        {
        case ::MeshRenderType::STATIC:
            meshes[MeshType::STATIC].push_back(component);
            break;

        case ::MeshRenderType::SKELETAL:
            meshes[MeshType::SKELTAL].push_back(component);
            break;
        }
    }

    UINT                  currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    ID3D12DescriptorHeap* dh                     = UmViewManager.GetShaderResourceHeap();
    ID3D12DescriptorHeap* hps[]                  = { dh, };

    auto                  resource               = dh->GetGPUDescriptorHandleForHeapStart();
    auto&                 frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];
    auto                  cameraData             = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();


    commandList->SetPipelineState(_psos[STATIC_ONE_SIDED].Get());
    commandList->SetGraphicsRootSignature(_shaders[MeshType::STATIC]->GetRootSignature());
    commandList->SetGraphicsRootConstantBufferView(_shaders[MeshType::STATIC]->GetRootParameterIndex("cameraData"), cameraData);

    frameResource->SetFrameResource(FrameResource::Type::TRANSFORM, _shaders[MeshType::STATIC]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResource::Type::MATERIAL, _shaders[MeshType::STATIC]->GetRootParameterIndex("material"), commandList);

    commandList->SetDescriptorHeaps(_countof(hps), hps);
    commandList->SetGraphicsRootDescriptorTable(_shaders[MeshType::STATIC]->GetRootParameterIndex("textures"), resource);
    DrawMeshes(commandList, meshes[MeshType::STATIC], MeshType::STATIC);

    // Skeletal Mesh
    commandList->SetPipelineState(_psos[SKELTAL_ONE_SIDED].Get());
    commandList->SetGraphicsRootSignature(_shaders[MeshType::SKELTAL]->GetRootSignature());
    commandList->SetGraphicsRootConstantBufferView(_shaders[MeshType::SKELTAL]->GetRootParameterIndex("cameraData"), cameraData);

    frameResource->SetFrameResource(FrameResource::Type::TRANSFORM, _shaders[MeshType::SKELTAL]->GetRootParameterIndex("worldMatrices"), commandList);
    frameResource->SetFrameResource(FrameResource::Type::BONE_MATRIXES, _shaders[MeshType::SKELTAL]->GetRootParameterIndex("boneMatrices"), commandList);
    frameResource->SetFrameResource(FrameResource::Type::MATERIAL, _shaders[MeshType::SKELTAL]->GetRootParameterIndex("material"), commandList);   

    DrawMeshes(commandList, meshes[MeshType::SKELTAL], MeshType::SKELTAL);
}

void GBufferPass::End(ID3D12GraphicsCommandList* commandList)
{
    const auto& gBufferGroup = UmMultiRenderTargetManager.GetRenderTargetGroup("GBuffer");
    for (auto& gBuffer : gBufferGroup)
    {
        gBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &br);
}

void GBufferPass::InitShaderAndPSO()
{
    _shaders.reserve(2);
    _psos.reserve(4);

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

    ID3D12Device*                      device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        staticTwoSidedPSO;

    psodesc.RasterizerState                        = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode               = D3D12_CULL_MODE_NONE;
    psodesc.BlendState                             = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                      = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask                             = UINT_MAX;
    psodesc.PrimitiveTopologyType                  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                            = _shaders[MeshType::STATIC]->GetInputLayout();
    psodesc.NumRenderTargets                       = GBuffer::GBUFFER_END;
    psodesc.RTVFormats[GBuffer::BASECOLOR]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::NORMAL]            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::ORM]               = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::EMISSIVE]          = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::WORLDPOSITION]     = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[GBuffer::DEPTH]             = DXGI_FORMAT_R32_FLOAT;
    psodesc.RTVFormats[GBuffer::CUSTOMDEPTH]       = DXGI_FORMAT_R32_UINT;
    psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature                         = _shaders[MeshType::STATIC]->GetRootSignature();
    psodesc.SampleDesc                             = {1, 0};
    psodesc.VS = _shaders[MeshType::STATIC]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS = _shaders[MeshType::STATIC]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&staticTwoSidedPSO));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(staticTwoSidedPSO);

    // static one side.
    ComPtr<ID3D12PipelineState> staticOneSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&staticOneSidePSO));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(staticOneSidePSO);

    // skeletal two side.
    ComPtr<ID3D12PipelineState> skeletalTwoSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psodesc.InputLayout              = _shaders[MeshType::SKELTAL]->GetInputLayout();
    psodesc.pRootSignature           = _shaders[MeshType::SKELTAL]->GetRootSignature();
    psodesc.VS                       = _shaders[MeshType::SKELTAL]->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                       = _shaders[MeshType::SKELTAL]->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&skeletalTwoSidePSO));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(skeletalTwoSidePSO);

    // skeletal one side.
    ComPtr<ID3D12PipelineState> skeletalOneSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr                               = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(&skeletalOneSidePSO));
    FAILED_CHECK_MESSAGE(hr, L"GBufferPass::InitShaderAndPSO device->CreateGraphicsPipelineState Failed");
    _psos.push_back(skeletalOneSidePSO);
}

void GBufferPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, const std::vector<MeshRenderer*>& meshes,
                             MeshType type)
{
    UINT param[3]{0, MAX_BONE_MATRIX, 0};
    for (auto& component : meshes)
    {
        const auto& model = component->GetModel();
        param[2]          = component->GetCustomDepth();

        for (auto& mesh : model->GetMeshes())
        {
            param[2] = 1; // 임시
            commandList->SetGraphicsRoot32BitConstants(_shaders[type]->GetRootParameterIndex("bit32_3_objectData"), 3, param, 0);
            param[0]++;
            mesh->Render(commandList);
        }
    }
}