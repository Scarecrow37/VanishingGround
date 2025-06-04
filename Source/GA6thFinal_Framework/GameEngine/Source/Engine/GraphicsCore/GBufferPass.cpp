#include "pch.h"
#include "GBufferPass.h"
#include "BaseMesh.h"
#include "FrameResource.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ShaderBuilder.h"

GBufferPass::~GBufferPass() {}

void GBufferPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);
    InitShaderAndPSO();
    _gbufferHandle.reserve(_ownerScene->_gBufferCount);
}

void GBufferPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _gbufferHandle.clear();
    // GBuffer -> RENDER_TARGET 전이 + Clear
    for (UINT i = 0; i < _ownerScene->_gBufferCount; ++i)
    {
        ComPtr<ID3D12Resource> gbuffer = _ownerScene->_gBuffer[i]->GetResource();

        CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(
            gbuffer.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &br);
        float                       clearValue = _ownerScene->_gBuffer[i]->clearValue;
        Color                       clearColor = {clearValue, clearValue, clearValue, 1.f};
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle  = _ownerScene->_gBuffer[i]->GetRTVHandle();
        commandList->ClearRenderTargetView(cpuHandle, clearColor, 0, nullptr);
    }

    // DepthStencil 상태 전이 + Clear
    CD3DX12_RESOURCE_BARRIER dsBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &dsBarrier);

    commandList->ClearDepthStencilView(_ownerScene->_depthStencilHandle,
                                       D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    // OM 세팅 (RTV + DSV)

    for (auto& rt : _ownerScene->_gBuffer)
    {
        _gbufferHandle.push_back(rt->GetRTVHandle());
    }
    commandList->OMSetRenderTargets(_ownerScene->_gBufferCount, _gbufferHandle.data(), FALSE,
                                    &_ownerScene->_depthStencilHandle);

    // 뷰포트 & 시저 설정
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
}

void GBufferPass::End(ID3D12GraphicsCommandList* commandList)
{
    CD3DX12_RESOURCE_BARRIER br;
    for (UINT i = 0; i < _ownerScene->_gBufferCount; ++i)
    {
        ComPtr<ID3D12Resource> gbuffer = _ownerScene->_gBuffer[i]->GetResource();
        br = CD3DX12_RESOURCE_BARRIER::Transition(gbuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &br);
    }
    br = CD3DX12_RESOURCE_BARRIER::Transition(_ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                              D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &br);
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
        case MESH_RENDER_TYPE::STATIC:
            meshes[MeshType::STATIC].push_back(component);
            break;

        case MESH_RENDER_TYPE::SKELETAL:
            meshes[MeshType::SKELTAL].push_back(component);
            break;
        }
    }

    UINT                         currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    ComPtr<ID3D12DescriptorHeap> dh    = _ownerScene->_frameResources[currentBackBufferIndex]->GetDescriptorHeap();
    ID3D12DescriptorHeap*        hps[] = {
        dh.Get(),
    };
    // 디스크립터-힙 설정.
    commandList->SetDescriptorHeaps(_countof(hps), hps);

    // 디스크립터-힙에서 첫번째 디스크립터 (배열)주소 획득.
    auto resource = dh->GetGPUDescriptorHandleForHeapStart();

    // ObjectData
    auto objectData = resource;

    // BoneTransform
    resource.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
    auto boneMatrices = resource;

    // MaterialData
    resource.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
    auto material = resource;

    // Textures
    resource.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
    auto textures = resource;

    commandList->SetGraphicsRootSignature(_shader[MeshType::STATIC]->GetRootSignature().Get());
    commandList->SetGraphicsRootConstantBufferView(_shader[MeshType::STATIC]->GetRootSignatureIndex("cameraData"),
                                                   _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::STATIC]->GetRootSignatureIndex("objectData"),
                                                objectData);
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::STATIC]->GetRootSignatureIndex("material"), material);
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::STATIC]->GetRootSignatureIndex("textures"), textures);
    
    commandList->SetPipelineState(_psos[STATIC_ONE_SIDED].Get());
    DrawMeshes(commandList, meshes[MeshType::STATIC], MeshType::STATIC);

    // SWTODO : 나중에 material 생성되면 material 별로 mesh 구분 후 찍어주기.
    // 그리기
    // DrawStaticTwoSidedMesh(commandList);
    // DrawStaticMeshes(commandList, _ownerScene->_staticTwoSidedMeshes);
    // DrawStaticMeshes(commandList, _ownerScene->_staticOneSidedMeshes);

    commandList->SetGraphicsRootSignature(_shader[MeshType::SKELTAL]->GetRootSignature().Get());
    commandList->SetGraphicsRootConstantBufferView(_shader[MeshType::SKELTAL]->GetRootSignatureIndex("cameraData"),
                                                   _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::SKELTAL]->GetRootSignatureIndex("objectData"),
                                                objectData);
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::SKELTAL]->GetRootSignatureIndex("boneMatrices"),
                                                boneMatrices);
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::SKELTAL]->GetRootSignatureIndex("material"),
                                                material);
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::SKELTAL]->GetRootSignatureIndex("textures"),
                                                textures);

    commandList->SetPipelineState(_psos[SKELTAL_ONE_SIDED].Get());
    DrawMeshes(commandList, meshes[MeshType::SKELTAL], MeshType::SKELTAL);
}

void GBufferPass::InitShaderAndPSO()
{
    _shader.reserve(2);
    _psos.reserve(4);

    std::shared_ptr<ShaderBuilder> staticMeshShaderBuilder = std::make_shared<ShaderBuilder>();
    staticMeshShaderBuilder->BeginBuild();
    staticMeshShaderBuilder->SetShader(L"../Shaders/vs_fr.hlsl", ShaderBuilder::Type::VS);
    staticMeshShaderBuilder->SetShader(L"../Shaders/ps_gbuffer.hlsl", ShaderBuilder::Type::PS);
    staticMeshShaderBuilder->EndBuild();
    _shader.push_back(staticMeshShaderBuilder);

    // SWTODO : Bone Matrix 생기면 vertex shader 바꿔주기.
    std::shared_ptr<ShaderBuilder> skeletalMeshShaderBuilder = std::make_shared<ShaderBuilder>();
    skeletalMeshShaderBuilder->BeginBuild();
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/vs_skeletal_fr.hlsl", ShaderBuilder::Type::VS);
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/ps_gbuffer.hlsl", ShaderBuilder::Type::PS);
    skeletalMeshShaderBuilder->EndBuild();
    _shader.push_back(skeletalMeshShaderBuilder);

    // static two side.
    ComPtr<ID3D12Device>               device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc{};
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        staticTwoSidedPSO;

    psodesc.RasterizerState                        = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode               = D3D12_CULL_MODE_NONE;
    psodesc.BlendState                             = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                      = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask                             = UINT_MAX;
    psodesc.PrimitiveTopologyType                  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                            = staticMeshShaderBuilder->GetInputLayout();
    psodesc.NumRenderTargets                       = RenderScene::GBuffer::END;
    psodesc.RTVFormats[RenderScene::BASECOLOR]     = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[RenderScene::NORMAL]        = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[RenderScene::ORM]           = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[RenderScene::EMISSIVE]      = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[RenderScene::WORLDPOSITION] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[RenderScene::DEPTH]         = DXGI_FORMAT_R32_FLOAT;
    psodesc.RTVFormats[RenderScene::CUSTOMDEPTH]   = DXGI_FORMAT_R32_UINT;
    psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature                         = staticMeshShaderBuilder->GetRootSignature().Get();
    psodesc.SampleDesc                             = {1, 0};
    psodesc.VS = staticMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS = staticMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(staticTwoSidedPSO.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(staticTwoSidedPSO);

    // static one side.
    ComPtr<ID3D12PipelineState> staticOneSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(staticOneSidePSO.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(staticOneSidePSO);

    // skeletal two side.
    ComPtr<ID3D12PipelineState> skeletalTwoSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psodesc.InputLayout              = skeletalMeshShaderBuilder->GetInputLayout();
    psodesc.pRootSignature           = skeletalMeshShaderBuilder->GetRootSignature().Get();
    psodesc.VS                       = skeletalMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                       = skeletalMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);

    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(skeletalTwoSidePSO.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(skeletalTwoSidePSO);

    // skeletal one side.
    ComPtr<ID3D12PipelineState> skeletalOneSidePSO;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(skeletalOneSidePSO.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(skeletalOneSidePSO);
}

void GBufferPass::DrawMeshes(ID3D12GraphicsCommandList* commandList, const std::vector<MeshRenderer*>& meshes,
                             MeshType type)
{    
    UINT param[2]{0, MAX_BONE_MATRIX};
    for (auto& component : meshes)
    {
        const auto& model = component->GetModel();
        for (auto& mesh : model->GetMeshes())
        {
            commandList->SetGraphicsRoot32BitConstant(_shader[type]->GetRootSignatureIndex("bit32_2_object"), param[0]++, 0);
            mesh->Render(commandList);
        }
    }
}

// void GBufferPass::DrawStaticTwoSidedMesh(ID3D12GraphicsCommandList* commandList)
//{
//     commandList->SetPipelineState(_psos[0].Get());
//
//     UINT ID = 0;
//     for (auto& [isDestroy, component] : _ownerScene->_renderQueue)
//     {
//         if (!component->IsActive())
//             continue;
//
//         const auto& model = component->GetModel();
//         for (auto& mesh : model->GetMeshes())
//         {
//             commandList->SetGraphicsRoot32BitConstant(_shader[0]->GetRootSignatureIndex("bit32_object"), ID++, 0);
//             mesh->Render(commandList);
//         }
//     }
// }
//
// void GBufferPass::DrawStaticMeshes(ID3D12GraphicsCommandList*                     commandList,
//                              const std::vector<std::pair<BaseMesh*, UINT>>& meshes)
//{
//     for (auto& [mesh,id] : meshes)
//     {
//         commandList->SetGraphicsRoot32BitConstant(_shader[MeshType::STATIC]->GetRootSignatureIndex("bit32_object"),
//         id, 0); mesh->Render(commandList);
//     }
// }
//
// void GBufferPass::DrawSkeletalMeshes(ID3D12GraphicsCommandList*                     commandList,
//                                      const std::vector<std::pair<BaseMesh*, UINT>>& meshes)
//{
//     for (auto& [mesh, id] : meshes)
//     {
//
//         commandList->SetGraphicsRoot32BitConstant(_shader[MeshType::SKELTAL]->GetRootSignatureIndex("bit32_object"),
//         id, 0); mesh->Render(commandList);
//     }
// }