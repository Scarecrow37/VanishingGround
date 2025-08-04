#include "pch.h"
#include "ParticleResolvePass.h"

ParticleResolvePass::ParticleResolvePass() {}

ParticleResolvePass::~ParticleResolvePass() {}

void ParticleResolvePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    InitializeShader();
    InitializePSO();
}

void ParticleResolvePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void ParticleResolvePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_shader->GetRootSignature());


    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("gAccumTex"), _accumlateBuffer->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootParameterIndex("gRevealTex"), _revealageBuffer->GetSRVHandle());

    _accumlateBuffer->ResourceBarrier(commandList);
    _revealageBuffer->ResourceBarrier(commandList);

    _ownerScene->_frameQuad->Render(commandList);
}

void ParticleResolvePass::End(ID3D12GraphicsCommandList* commandList)
{
    _accumlateBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _revealageBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ParticleResolvePass::SetAccumulationBuffers(SharedResource<UnorderedAccessView> color, SharedResource<UnorderedAccessView> alpha)
{
    _accumlateBuffer = color;
    _revealageBuffer = alpha;
}

void ParticleResolvePass::InitializeShader()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/vs_quad.hlsl", ShaderBuilder::Type::VS);
    _shader->SetShader(L"../Shaders/ps_particle_resolve.hlsl", ShaderBuilder::Type::PS);
    _shader->EndBuild();
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
    psoDesc.pRootSignature                = _shader->GetRootSignature();
    psoDesc.VS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::VS);
    psoDesc.PS                            = _shader->GetShaderByteCode(ShaderBuilder::Type::PS);
    psoDesc.InputLayout                   = _shader->GetInputLayout();
    psoDesc.RTVFormats[0]                 = DXGI_FORMAT_R32G32B32A32_FLOAT;
    psoDesc.NumRenderTargets              = 1;
    psoDesc.SampleMask                    = UINT_MAX;
    psoDesc.RasterizerState               = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    //psoDesc.BlendState                    = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.BlendState                    = blendDesc;
    psoDesc.DepthStencilState             = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.SampleDesc.Count              = 1;

    ComPtr<ID3D12Device> device = Global::device->GetDevice();
    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState));
}