#include "pch.h"
#include "GBufferPass.h"
#include "BaseMesh.h"
#include "FrameResource.h"
#include "Model.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ShaderBuilder.h"
#include "MeshRenderer.h"

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
            gbuffer.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &br);

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle  = _ownerScene->_gBuffer[i]->GetRTVHandle();
        commandList->ClearRenderTargetView(cpuHandle, _clearColor, 0, nullptr);
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
    UINT                         currentBackBufferIndex = UmDevice.GetCurrentBackBufferIndex();
    ComPtr<ID3D12DescriptorHeap> dh    = _ownerScene->_frameResources[currentBackBufferIndex]->GetDescriptorHeap();
    ID3D12DescriptorHeap*        hps[] = {
        dh.Get(),
    };
    // 디스크립터-힙 설정.
    commandList->SetDescriptorHeaps(_countof(hps), hps);

    // 디스크립터-힙에서 첫번째 디스크립터 (배열)주소 획득.
    D3D12_GPU_DESCRIPTOR_HANDLE resource = dh->GetGPUDescriptorHandleForHeapStart();
    D3D12_GPU_DESCRIPTOR_HANDLE objectData = resource;
    resource.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
    D3D12_GPU_DESCRIPTOR_HANDLE material = resource;
    //resource.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
    //D3D12_GPU_DESCRIPTOR_HANDLE boneMatrix = resource;
    resource.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
    D3D12_GPU_DESCRIPTOR_HANDLE textures = resource;
    commandList->SetGraphicsRootSignature(_shader[MeshType::STATIC]->GetRootSignature().Get());

    //  현재 타겟의 카메라 버퍼 설정
    commandList->SetGraphicsRootConstantBufferView(_shader[MeshType::STATIC]->GetRootSignatureIndex("cameraData"),
                                                   _ownerScene->_cameraBuffer->GetGPUVirtualAddress());

    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::STATIC]->GetRootSignatureIndex("objectData"),
                                                objectData);
    
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::STATIC]->GetRootSignatureIndex("material"), material);
    
    commandList->SetGraphicsRootDescriptorTable(_shader[MeshType::STATIC]->GetRootSignatureIndex("textures"), textures);

    // SWTODO : 나중에 material 생성되면 material 별로 mesh 구분 후 찍어주기.
    // 그리기
    DrawStaticTwoSidedMesh(commandList);
    //DrawStaticMeshes(commandList, _ownerScene->_staticTwoSidedMeshes);
    //DrawStaticMeshes(commandList, _ownerScene->_staticOneSidedMeshes);

    //commandList->SetGraphicsRootConstantBufferView(_shaders[MeshType::SKELTAL]->GetRootSignatureIndex("cameraData"),
    //                                               _ownerScene->_cameraBuffer->GetGPUVirtualAddress());

    //commandList->SetGraphicsRootDescriptorTable(_shaders[MeshType::SKELTAL]->GetRootSignatureIndex("objectData"),
    //                                            objectData);

    //commandList->SetGraphicsRootDescriptorTable(_shaders[MeshType::SKELTAL]->GetRootSignatureIndex("material"),
    //                                            material);
    
    //commandList->SetGraphicsRootDescriptorTable(_shaders[MeshType::SKELTAL]->GetRootSignatureIndex("boneMatrix"),
    //                                             boneMatrix);

    //commandList->SetGraphicsRootDescriptorTable(_shaders[MeshType::SKELTAL]->GetRootSignatureIndex("textures"),
    //                                            textures);
    // DrawStaticMeshes(commandList, _ownerScene->_skeletalTwoSidedMeshes);
    // DrawStaticMeshes(commandList, _ownerScene->_skeletalOneSidedMeshes);
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
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/vs_fr.hlsl", ShaderBuilder::Type::VS);
    skeletalMeshShaderBuilder->SetShader(L"../Shaders/ps_gbuffer.hlsl", ShaderBuilder::Type::PS);
    skeletalMeshShaderBuilder->EndBuild();
    _shader.push_back(skeletalMeshShaderBuilder);
   
    // static two side.
    ComPtr<ID3D12Device> device      = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    HRESULT hr = S_OK;
    ComPtr<ID3D12PipelineState>        statictwosidedpso;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psodesc.BlendState               = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState        = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.SampleMask               = UINT_MAX;
    psodesc.PrimitiveTopologyType    = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout              = staticMeshShaderBuilder->GetInputLayout();
    psodesc.NumRenderTargets         = 7;
    psodesc.RTVFormats[0]            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[1]            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[2]            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[3]            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[4]            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.RTVFormats[5]            = DXGI_FORMAT_R32_FLOAT;
    psodesc.RTVFormats[6]            = DXGI_FORMAT_R32_UINT;
    psodesc.DSVFormat                = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature           = staticMeshShaderBuilder->GetRootSignature().Get();
    psodesc.SampleDesc               = {1, 0};
    psodesc.VS                       = staticMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                       = staticMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);
    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(statictwosidedpso.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(statictwosidedpso);

    //static one side.
    ComPtr<ID3D12PipelineState> staticonesidepso;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(staticonesidepso.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(staticonesidepso);

    // skeletal two side.
    ComPtr<ID3D12PipelineState> skeletaltwosidepso;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psodesc.InputLayout              = skeletalMeshShaderBuilder->GetInputLayout();
    psodesc.pRootSignature           = skeletalMeshShaderBuilder->GetRootSignature().Get();
    psodesc.VS                       = skeletalMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS                       = skeletalMeshShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);
    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(skeletaltwosidepso.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(skeletaltwosidepso);

    // skeletal one side.
    ComPtr<ID3D12PipelineState> skeletalonesidepso;
    psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    hr = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(skeletalonesidepso.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(skeletalonesidepso);

}

void GBufferPass::DrawStaticTwoSidedMesh(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_psos[0].Get());

    UINT ID = 0;
    for (auto& component : _ownerScene->_renderQueue)
    {
        if (!component->IsActive())
            continue;
        
        const auto& model = component->GetModel();
        for (auto& mesh : model->GetMeshes())
        {
            commandList->SetGraphicsRoot32BitConstant(_shader[0]->GetRootSignatureIndex("bit32_object"), ID++, 0);
            mesh->Render(commandList);
        }
    }
}

void GBufferPass::DrawStaticMeshes(ID3D12GraphicsCommandList*                     commandList,
                             const std::vector<std::pair<BaseMesh*, UINT>>& meshes)
{
    for (auto& [mesh,id] : meshes)
    {        
        commandList->SetGraphicsRoot32BitConstant(_shader[MeshType::STATIC]->GetRootSignatureIndex("bit32_object"), id, 0);
        mesh->Render(commandList);
    }
}

void GBufferPass::DrawSkeletalMeshes(ID3D12GraphicsCommandList*                     commandList,
                                     const std::vector<std::pair<BaseMesh*, UINT>>& meshes)
{
    for (auto& [mesh, id] : meshes)
    {

        commandList->SetGraphicsRoot32BitConstant(_shader[MeshType::SKELTAL]->GetRootSignatureIndex("bit32_object"), id, 0);
        mesh->Render(commandList);
    }
}
