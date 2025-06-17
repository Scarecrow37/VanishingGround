#include "pch.h"
#include "ParticleSpritePass.h"
#include "ShaderBuilder.h"
#include "RenderTarget.h"
#include "RenderScene.h"
#include "Model.h"
#include "Texture.h"
#include "Quad.h"

 ParticleSpritePass::~ParticleSpritePass() {}

void ParticleSpritePass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    __super::Initialize(viewPort, sissorRect);
    _particleQuad = UmResourceManager.LoadResource<Model>("Quad");

    
    InitializeShader();
    InitializePSO();
    InitializeDescriptorHeap();

    //refactoring needed
    _particleRenderCommandList = UmParticleManager.GetRenderCommandList();



}

void ParticleSpritePass::Begin(ID3D12GraphicsCommandList* commandlist)
{

    UmParticleManager.ResetRenderCommandObject();
    _particleRenderCommandList->OMSetRenderTargets(1, &_ownerScene->_meshLightingTarget->GetRTVHandle(), FALSE,
                                                   &_ownerScene->_depthStencilHandle);
    _particleRenderCommandList->RSSetViewports(1, &_viewPort);
    _particleRenderCommandList->RSSetScissorRects(1, &_sissorRect);

    //float blendfactor[] = {0.f, 0.f, 0.f, 0.f};
    //float blendfactor[] = {0.5f, 0.5f, 0.5f, 0.5f};
    float blendfactor[] = {1, 1, 1, 1};
    _particleRenderCommandList->OMSetBlendFactor(blendfactor);
    ComPtr<ID3D12Resource> resource = UmParticleManager.GetComputeOutputResource();

    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    _particleRenderCommandList->ResourceBarrier(1, &computeOutputBarrior);

        CD3DX12_RESOURCE_BARRIER dsBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    _particleRenderCommandList->ResourceBarrier(1, &dsBarrier);


    auto albedoTextures = UmParticleManager.GetActiveAlbedos();
    auto device         = UmDevice.GetDevice();
    D3D12_CPU_DESCRIPTOR_HANDLE heapStart      = _descriptorheap->GetCPUDescriptorHandleForHeapStart();

    for (int i = 0; i < albedoTextures.size();++i)
    {
        // 각 Texture 객체의 SRV CPU 핸들 얻기
        D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = albedoTextures[i]->GetCPUHandle();

        // 디스크립터 힙 내 복사 위치 계산
        D3D12_CPU_DESCRIPTOR_HANDLE destHandle = heapStart;
        destHandle.ptr += _descriptorSize * i; // 0번은 StructuredBuffer SRV라면 1번부터 텍스처

        // 디스크립터 복사
        device->CopyDescriptorsSimple(1,          // 복사할 디스크립터 개수
                                      destHandle, // 목적지(디스크립터 힙 내)
                                      srcHandle,  // 소스(각 Texture의 SRV)
                                      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }




}

void ParticleSpritePass::End(ID3D12GraphicsCommandList* commandlist)
{

    ComPtr<ID3D12Resource>   resource             = UmParticleManager.GetComputeOutputResource();
    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);

    CD3DX12_RESOURCE_BARRIER meshLightingBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        _ownerScene->_meshLightingTarget->GetResource() , D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    D3D12_RESOURCE_BARRIER barriors[] = {computeOutputBarrior, meshLightingBarrior};

    _particleRenderCommandList->ResourceBarrier(2, barriors);

    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(
        _ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PRESENT);

    _particleRenderCommandList->ResourceBarrier(1, &br);
    
}

void ParticleSpritePass::Draw(ID3D12GraphicsCommandList* commandlist)
{


    ComPtr<ID3D12Device> device         = UmDevice.GetDevice();
    ID3D12DescriptorHeap* hps[] = {
        _descriptorheap.Get(),
    };


    _particleRenderCommandList->SetDescriptorHeaps(_countof(hps), hps);
    D3D12_GPU_DESCRIPTOR_HANDLE descHeapPtr = _descriptorheap->GetGPUDescriptorHandleForHeapStart();
    
    _particleRenderCommandList->SetGraphicsRootSignature(_spriteParticleShaderBuilder->GetRootSignature());
    // StructuredBuffer (t0)
    auto outputResource = UmParticleManager.GetComputeOutputResource();
    _particleRenderCommandList->SetGraphicsRootShaderResourceView(0, outputResource->GetGPUVirtualAddress());


    //// GPU 핸들 계산 (1번 슬롯부터 연속된 텍스처 테이블)
    //descHeapPtr.ptr += _descriptorSize * 1; // 0번이 StructuredBuffer라면 1번부터

    // 디스크립터 테이블 바인딩 (루트 파라미터 인덱스는 RootSignature에 따라 다름)
    _particleRenderCommandList->SetGraphicsRootDescriptorTable(
        _spriteParticleShaderBuilder->GetRootParameterIndex("AlbedoTextures"), descHeapPtr);

    _particleRenderCommandList->SetPipelineState(_psos[0].Get());

    const auto& meshs = _particleQuad->GetMeshes();
    UINT        totalCount = UmParticleManager.GetTotalCount();
    meshs[0]->Render(_particleRenderCommandList, totalCount);

}

void ParticleSpritePass::InitializeShader() 
{
    _spriteParticleShaderBuilder = std::make_shared<ShaderBuilder>();
    _spriteParticleShaderBuilder->BeginBuild();
    _spriteParticleShaderBuilder->SetShader(L"../Shaders/vs_particle_quad.hlsl", ShaderBuilder::Type::VS);
    _spriteParticleShaderBuilder->SetShader(L"../Shaders/ps_particle_quad.hlsl", ShaderBuilder::Type::PS);
    _spriteParticleShaderBuilder->EndBuild();
}

void ParticleSpritePass::InitializePSO() 
{


    D3D12_BLEND_DESC blendDesc       = {};
    blendDesc.AlphaToCoverageEnable  = FALSE;
    blendDesc.IndependentBlendEnable = TRUE;
    auto& rtDesc                     = blendDesc.RenderTarget[0];
    rtDesc.BlendEnable               = TRUE;
    rtDesc.SrcBlend                  = D3D12_BLEND_SRC_ALPHA;
    rtDesc.DestBlend                 = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp                   = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha             = D3D12_BLEND_ONE;
    rtDesc.DestBlendAlpha            = D3D12_BLEND_ONE;
    rtDesc.BlendOpAlpha              = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask     = D3D12_COLOR_WRITE_ENABLE_ALL;
    


    // static two side.
    ComPtr<ID3D12Device>               device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        statictwosidedpso;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState                        = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode               = D3D12_CULL_MODE_NONE;
    //psodesc.BlendState                             = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.BlendState                             = blendDesc;
    psodesc.DepthStencilState                      = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState.DepthEnable          = FALSE;
    //psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;

    psodesc.SampleMask                             = UINT_MAX;
    psodesc.PrimitiveTopologyType                  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                            = _spriteParticleShaderBuilder->GetInputLayout();
    psodesc.NumRenderTargets                       = 1;
    psodesc.RTVFormats[0]                          = DXGI_FORMAT_R32G32B32A32_FLOAT;
    //psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature                         = _spriteParticleShaderBuilder->GetRootSignature();
    psodesc.SampleDesc                             = {1, 0};
    psodesc.VS = _spriteParticleShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS = _spriteParticleShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(statictwosidedpso.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"");
    _psos.push_back(statictwosidedpso);

}

void ParticleSpritePass::InitializeDescriptorHeap() 
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 101; // max emitter count *2 + structured buffer srv
    heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    FAILED_CHECK_MESSAGE(
        UmDevice.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_descriptorheap.GetAddressOf())), L"");

    _descriptorSize = UmDevice.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //CD3DX12_CPU_DESCRIPTOR_HANDLE handle(_descriptorheap->GetCPUDescriptorHandleForHeapStart());
    //D3D12_SHADER_RESOURCE_VIEW_DESC particleOutputSrvDesc{};
    //particleOutputSrvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
    //particleOutputSrvDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
    //particleOutputSrvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    //particleOutputSrvDesc.Buffer.FirstElement        = 0;
    //particleOutputSrvDesc.Buffer.NumElements         = UmParticleManager.GetMaxCount();
    //particleOutputSrvDesc.Buffer.StructureByteStride = sizeof(ParticleOutput);
    //particleOutputSrvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
    //ID3D12Resource* resource                         = UmParticleManager.GetComputeOutputResource().Get();
    //UmDevice.GetDevice()->CreateShaderResourceView(resource, &particleOutputSrvDesc, handle);

}
