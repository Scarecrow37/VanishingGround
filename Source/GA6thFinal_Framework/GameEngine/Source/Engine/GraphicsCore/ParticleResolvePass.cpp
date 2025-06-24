#include "pch.h"
#include "Quad.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ShaderBuilder.h"
#include "ParticleResolvePass.h"
#include "UnorderedAccessView.h"

 ParticleResolvePass::ParticleResolvePass() {}

ParticleResolvePass::~ParticleResolvePass() {}

void ParticleResolvePass::Initialize(const D3D12_VIEWPORT& viewport, const D3D12_RECT& scissorRect)
{
    __super::Initialize(viewport, scissorRect);
    InitializeShader();
    InitializePSO();
    InitializeDescriptorHeap();
    _particleRenderCommandList = UmParticleManager.GetRenderCommandList();
}

void ParticleResolvePass::Begin(ID3D12GraphicsCommandList* commandlist)
{
    _meshRenderTarget->TransitionResource(_particleRenderCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                                          D3D12_RESOURCE_STATE_RENDER_TARGET);

    _particleRenderCommandList->RSSetViewports(1, &_viewPort);
    _particleRenderCommandList->RSSetScissorRects(1, &_sissorRect);

    _meshRenderTarget->ClearRenderTarget(_particleRenderCommandList);
    _particleRenderCommandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
}

void ParticleResolvePass::End(ID3D12GraphicsCommandList* commandlist)
{

    _accumlateBuffer->TransitionResource(_particleRenderCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _revealageBuffer->TransitionResource(_particleRenderCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    _meshRenderTarget->TransitionResource(_particleRenderCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET,
                                          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ParticleResolvePass::Draw(ID3D12GraphicsCommandList* commandlist)
{
    ComPtr<ID3D12Device>  device = UmDevice.GetDevice();
    ID3D12DescriptorHeap* hps[]  = {
        UmViewManager.GetShaderResourceHeap(),
    };
    _particleRenderCommandList->SetDescriptorHeaps(_countof(hps), hps);
    _particleRenderCommandList->SetPipelineState(_resolvePSO.Get());
    _particleRenderCommandList->SetGraphicsRootSignature(_resolveShaderBuilder->GetRootSignature());

    _particleRenderCommandList->SetGraphicsRootDescriptorTable(
        _resolveShaderBuilder->GetRootParameterIndex("gAccumTex"), _accumlateBuffer->GetSRVHandle());
    _particleRenderCommandList->SetGraphicsRootDescriptorTable(
        _resolveShaderBuilder->GetRootParameterIndex("gRevealTex"), _revealageBuffer->GetSRVHandle());

    _particleRenderCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    _accumlateBuffer->ResourceBarrier(_particleRenderCommandList);
    _revealageBuffer->ResourceBarrier(_particleRenderCommandList);
    _ownerScene->_frameQuad->Render(_particleRenderCommandList);




}

void ParticleResolvePass::SetAccumulationBuffers(UnorderedAccessView* color, UnorderedAccessView* alpha)
{
        _accumlateBuffer = color;
        _revealageBuffer = alpha;
}

void ParticleResolvePass::InitializeShader()
{
    _resolveShaderBuilder = std::make_shared<ShaderBuilder>();
    _resolveShaderBuilder->BeginBuild();

    _resolveShaderBuilder->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);

    _resolveShaderBuilder->SetShader(L"../Shaders/ps_particle_resolve.hlsl", ShaderBuilder::Type::PS);
    _resolveShaderBuilder->EndBuild();
}

void ParticleResolvePass::InitializePSO()
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
    

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    ZeroMemory(&psoDesc, sizeof(psoDesc));
    psoDesc.InputLayout                   = {nullptr, 0}; // No input layout
    psoDesc.PrimitiveTopologyType         = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.pRootSignature                = _resolveShaderBuilder->GetRootSignature();
    psoDesc.VS                            = _resolveShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::VS);
    psoDesc.PS                            = _resolveShaderBuilder->GetShaderByteCode(ShaderBuilder::Type::PS);
    psoDesc.InputLayout                   = _resolveShaderBuilder->GetInputLayout();
    psoDesc.RTVFormats[0]                 = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psoDesc.NumRenderTargets              = 1;
    psoDesc.SampleMask                    = UINT_MAX;
    psoDesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    //psoDesc.BlendState                    = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.BlendState                    = blendDesc;
    psoDesc.DepthStencilState             = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.SampleDesc.Count              = 1;

    ComPtr<ID3D12Device> device = UmDevice.GetDevice();
    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_resolvePSO));
}

void ParticleResolvePass::InitializeDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors             = 2; // AccumColor + AccumAlpha
    heapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ComPtr<ID3D12Device> device = UmDevice.GetDevice();
    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_descriptorHeap));

    _descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}