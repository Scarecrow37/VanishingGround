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

    //test
    //_testSprite->LoadResource("../../defaultSmoke.jpg");
    
    
    InitializeShader();
    InitializePSO();
    InitializeDescriptorHeap();




    //test
    UmParticleManager.RegisterEffect();





}

void ParticleSpritePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->OMSetRenderTargets(1, &_ownerScene->_meshLightingTarget->GetRTVHandle(), FALSE, &_ownerScene->_depthStencilHandle);
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
    ComPtr<ID3D12Resource> resource = UmParticleManager.GetComputeOutputResource();

    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    commandList->ResourceBarrier(1, &computeOutputBarrior);

        CD3DX12_RESOURCE_BARRIER dsBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &dsBarrier);



}

void ParticleSpritePass::End(ID3D12GraphicsCommandList* commandList)
{
    ComPtr<ID3D12Resource>   resource             = UmParticleManager.GetComputeOutputResource();
    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);

    CD3DX12_RESOURCE_BARRIER meshLightingBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        _ownerScene->_meshLightingTarget->GetResource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    D3D12_RESOURCE_BARRIER barriors[] = {computeOutputBarrior, meshLightingBarrior};

    commandList->ResourceBarrier(2, barriors);

    CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(
        _ownerScene->_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PRESENT);

    commandList->ResourceBarrier(1, &br);


}

void ParticleSpritePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    ComPtr<ID3D12Device> device         = UmDevice.GetDevice();
    ID3D12DescriptorHeap* hps[] = {
        _descriptorheap.Get(),
    };


    commandList->SetDescriptorHeaps(_countof(hps), hps);
    D3D12_GPU_DESCRIPTOR_HANDLE descHeapPtr = _descriptorheap->GetGPUDescriptorHandleForHeapStart();

    commandList->SetGraphicsRootSignature(_spriteParticleShaderBuilder->GetRootSignature().Get());
    // StructuredBuffer (t0)
    commandList->SetGraphicsRootDescriptorTable(0, descHeapPtr);
    commandList->SetPipelineState(_psos[0].Get());


    //// Texture2D (t1)
    //descHeapPtr.ptr += _descriptorSize;
    //commandList->SetGraphicsRootDescriptorTable(1, descHeapPtr);



    
    
    const auto& meshs = _particleQuad->GetMeshes();
    meshs[0]->Render(commandList, UmParticleManager.GetTotalCount());








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
    // static two side.
    ComPtr<ID3D12Device>               device = UmDevice.GetDevice();
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc;
    HRESULT                            hr = S_OK;
    ComPtr<ID3D12PipelineState>        statictwosidedpso;
    ZeroMemory(&psodesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psodesc.RasterizerState                        = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psodesc.RasterizerState.CullMode               = D3D12_CULL_MODE_NONE;
    psodesc.BlendState                             = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psodesc.DepthStencilState                      = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;

    psodesc.SampleMask                             = UINT_MAX;
    psodesc.PrimitiveTopologyType                  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psodesc.InputLayout                            = _spriteParticleShaderBuilder->GetInputLayout();
    psodesc.NumRenderTargets                       = 1;
    psodesc.RTVFormats[0]                          = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psodesc.DSVFormat                              = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psodesc.pRootSignature                         = _spriteParticleShaderBuilder->GetRootSignature().Get();
    psodesc.SampleDesc                             = {1, 0};
    psodesc.VS = _spriteParticleShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psodesc.PS = _spriteParticleShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);
    hr         = device->CreateGraphicsPipelineState(&psodesc, IID_PPV_ARGS(statictwosidedpso.GetAddressOf()));
    FAILED_CHECK_BREAK(hr);
    _psos.push_back(statictwosidedpso);

}

void ParticleSpritePass::InitializeDescriptorHeap() 
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 201; // max emitter count *2 + structured buffer srv
    heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    FAILED_CHECK_BREAK(
        UmDevice.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_descriptorheap.GetAddressOf())));

    _descriptorSize = UmDevice.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(_descriptorheap->GetCPUDescriptorHandleForHeapStart());
    D3D12_SHADER_RESOURCE_VIEW_DESC particleOutputSrvDesc{};
    particleOutputSrvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
    particleOutputSrvDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
    particleOutputSrvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    particleOutputSrvDesc.Buffer.FirstElement        = 0;
    particleOutputSrvDesc.Buffer.NumElements         = UmParticleManager.GetMaxCount();
    particleOutputSrvDesc.Buffer.StructureByteStride = sizeof(Particle);
    particleOutputSrvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
    ID3D12Resource* resource                         = UmParticleManager.GetComputeOutputResource().Get();
    UmDevice.GetDevice()->CreateShaderResourceView(resource, &particleOutputSrvDesc, handle);


    ////test
    //handle.Offset(1, _descriptorSize);
    //D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = _testSprite->GetHandle(); 
    //UmDevice.GetDevice()->CopyDescriptorsSimple
    //(
    //    1,
    //    handle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    //);




}
